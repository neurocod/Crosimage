#include "pch.h"
#include "ThumbWorker.h"
#include <cstdio>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

// Expose the existing job type without constructing another worker or changing
// the production interface. The test exercises its real step/requeue contract.
class WorkerJobAccess: public ThumbWorker {
public:
	using ThumbWorker::GenerateDirRecursive;
};

static bool check(bool condition, const char *message) {
	if(!condition)
		fprintf(stderr, "FAIL: %s\n", message);
	return condition;
}

static bool runRecursiveJob(ThumbWorker &worker, const QDir &dir,
	const QStringList &expectedImages)
{
	QSet<QString> decodedImages;
	int updates = 0;
	const auto connection = QObject::connect(&worker, &ThumbWorker::updated,
		[&](const QString &path, const QImage &image) {
			++updates;
			if(!image.isNull())
				decodedImages.insert(path);
		});
	WorkerJobAccess::GenerateDirRecursive job(dir, QDirIterator::Subdirectories);
	int steps = 0;
	do {
		job.run(&worker);
		++steps;
	} while(!job._delete && steps < 256);
	bool ok = check(job._delete, "recursive job must finish after exhausting the directory");
	for(const QString &path: expectedImages)
		ok = check(decodedImages.contains(path), "recursive job must decode every fixture image") && ok;
	if(expectedImages.isEmpty())
		ok = check(updates == 0, "empty directory must not publish phantom entries") && ok;
	const int previousUpdates = updates;
	job.run(&worker);
	ok = check(job._delete && updates == previousUpdates,
		"an exhausted iterator must stay finished without generating another path") && ok;
	QObject::disconnect(connection);
	return ok;
}

int main(int argc, char **argv) {
	QCoreApplication application(argc, argv);
	if(argc != 2) {
		fprintf(stderr, "Usage: worker_regression EMPTY_TEMP_DIRECTORY\n"
			"Remove the directory after this process exits (DirDb retains connections).\n");
		return 2;
	}
	QDir root(QString::fromLocal8Bit(argv[1]));
	if(!check(root.exists() && root.entryList(QDir::AllEntries | QDir::NoDotAndDotDot
		| QDir::Hidden | QDir::System).isEmpty(), "fixture directory must exist and be empty"))
		return 2;
	if(!check(root.mkpath("empty") && root.mkpath("populated/nested"),
		"fixture subdirectories must be created"))
		return 2;
	QImage fixture(12, 9, QImage::Format_RGB32);
	fixture.fill(Qt::red);
	const QString first = root.absoluteFilePath("populated/first.png");
	const QString second = root.absoluteFilePath("populated/nested/second.png");
	if(!check(fixture.save(first) && fixture.save(second), "fixture images must be saved"))
		return 2;

	auto &worker = ThumbWorker::instance();
	bool ok = true;
	// Keep all database creation and recursive processing on one background
	// thread. The actual worker's queue stays empty throughout this job test.
	QThread *testThread = QThread::create([&]() {
		ok = runRecursiveJob(worker, QDir(root.absoluteFilePath("empty")), {}) && ok;
		ok = runRecursiveJob(worker, QDir(root.absoluteFilePath("populated")), {first, second}) && ok;
	});
	testThread->start();
	testThread->wait();
	delete testThread;
	worker.exit();
	worker.wait();

	// Check allocations after both threads have stopped, so their startup or
	// teardown cannot affect the Debug CRT baseline.
	const QString missingPath = root.absoluteFilePath("not-queued.png");
	for(int i = 0; i < 100; ++i)
		worker.makeFirst(missingPath);
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtMemState before = {}, after = {};
	_CrtMemCheckpoint(&before);
	for(int i = 0; i < 10000; ++i)
		worker.makeFirst(missingPath);
	_CrtMemCheckpoint(&after);
	ok = check(after.lCounts[_NORMAL_BLOCK] == before.lCounts[_NORMAL_BLOCK]
		&& after.lSizes[_NORMAL_BLOCK] == before.lSizes[_NORMAL_BLOCK],
		"repeated makeFirst calls must not retain allocations") && ok;
#else
	fprintf(stdout, "SKIP: makeFirst allocation check requires MSVC Debug CRT\n");
#endif
	if(ok)
		fprintf(stdout, "PASS: thumbnail prioritization allocations and recursive job exhaustion\n");
	return ok ? 0 : 1;
}
