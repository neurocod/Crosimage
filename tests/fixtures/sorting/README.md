# Sorting and navigation fixtures

Folders `A`, `B`, and `C` contain the same three labeled images. Their names
(`image1.jpg`, `image2.png`, `image10.png`), extensions, dimensions, and file sizes
differ, making them useful for sorting, thumbnails, and navigation checks.
The images are generated test assets, with no external source dependencies.

From the repository root, build and capture a folder:

```powershell
python build.py --run --folder tests/fixtures/sorting/A
```

For interactive checks with isolated application settings:

```powershell
.\build\x64\bin\Debug\Crosimage.exe --screenshot --folder tests/fixtures/sorting/A --manual-run
```

Check folder sorting preferences:

1. In `A`, choose `Name`. Expect `image1.jpg`, `image2.png`, `image10.png`.
   Enable `Reversed`; expect the opposite order.
2. Use the sibling navigation buttons to visit `B`. If it has no saved
   preference, it should keep the current sorting. Choose `Type` and turn off
   `Reversed`; JPG should precede PNG.
3. Return to `A`. Both `Name` and `Reversed` should be restored, including the
   controls and the displayed order. Return to `B` and check its own preference.
4. Press `F5`, then close and reopen the viewer. The preference should survive
   refresh and restart. Repeat with `Time` and `Size`, in both directions.
5. Use `C` to check navigation to a folder without a saved preference. Merely
   visiting a folder should not save an inherited preference.

Preferences are stored as the integer `sortFlags` value in the `settings`
table of each folder's `thumbnails.sqlite`. These generated databases and their
SQLite sidecar files are ignored by Git. To reset a folder, close the viewer and
delete its `thumbnails.sqlite` (the thumbnails will be regenerated).

Git does not preserve modification times. Before testing `Time`, set distinct
timestamps explicitly, for example:

```powershell
$fixture = 'tests/fixtures/sorting/A'
(Get-Item "$fixture/image1.jpg").LastWriteTime = [datetime]'2024-01-01'
(Get-Item "$fixture/image2.png").LastWriteTime = [datetime]'2024-01-02'
(Get-Item "$fixture/image10.png").LastWriteTime = [datetime]'2024-01-03'
```
