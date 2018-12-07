//HttpStatusCode.h by Kostya Kozachuck as neurocod - 2015.12.01 07:24:06
#pragma once

enum HttpStatusCode {
	Http_200_OK = 200,
	Http_201_Created = 201,
	Http_202_Accepted = 202,
	Http_203_Non_Authoritative_Information = 203,
	Http_204_No_Content = 204,//The server successfully processed the request, but is not returning any content.
	Http_205_Reset_Content = 205,//The server successfully processed the request, but is not returning any content.Unlike a 204 response, this response requires that the requester reset the document view.
	Http_206_Partial_Content = 206, //(RFC 7233) The server is delivering only part of the resource(byte serving) due to a range header sent by the client.The range header is used by HTTP clients to enable resuming of interrupted downloads, or split a download into multiple simultaneous streams.
	Http_207_Multi_Status = 207,//(WebDAV; RFC 4918) The message body that follows is an XML message and can contain a number of separate response codes, depending on how many sub-requests were made.
	Http_208_Already_Reported = 208,//(WebDAV; RFC 5842) The members of a DAV binding have already been enumerated in a previous reply to this request, and are not being included again.
	Http_226_IM_Used = 226,//(RFC 3229) The server has fulfilled a request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance.[19]
	//3 3xx Redirection This class of status code indicates the client must take additional action to complete the request.Many of these status codes are used in URL redirection.
	//A user agent may carry out the additional action with no user interaction only if the method used in the second request is GET or HEAD.A user agent should not automatically redirect a request more than five times[citation needed], since such redirections usually indicate an infinite loop.
	Http_300_Multiple_Choices = 300,//Indicates multiple options for the resource that the client may follow.It, for instance, could be used to present different format options for video, list files with different extensions, or word sense disambiguation.
	Http_301_Moved_Permanently = 301,//This and all future requests should be directed to the given URI.
	Http_302_Redirect = 302,//302 Found This is an example of industry practice contradicting the standard. The  HTTP/1.0 specification (RFC 1945) required the client to perform a temporary redirect (the original describing phrase was "Moved Temporarily"), but popular browsers implemented 302 with the functionality of a 303 See Other.Therefore, HTTP/1.1 added status codes 303 and 307 to distinguish between the two behaviours.[25] However, some Web applications and frameworks use the 302 status code as if it were the 303.[26]
	Http_303_See_Other = 303,//(since HTTP/1.1) The response to the request can be found under another URI using a GET method.When received in response to a POST(or PUT/DELETE), it should be assumed that the server has received the data and the redirect should be issued with a separate GET message.
	Http_304_Not_Modified = 304,//(RFC 7232) Indicates that the resource has not been modified since the version specified by the request headers If-Modified-Since or If-None-Match.This means that there is no need to retransmit the resource, since the client still has a previously-downloaded copy.
	Http_305_Use_Proxy = 305,//(since HTTP/1.1) The requested resource is only available through a proxy, whose address is provided in the response.Many HTTP clients(such as Mozilla[29] and Internet Explorer) do not correctly handle responses with this status code, primarily for security reasons.
	Http_306_Switch_Proxy = 306,//No longer used. Originally meant "Subsequent requests should use the specified proxy."
	Http_307_Temporary_Redirect = 307,//(since HTTP/1.1) In this case, the request should be repeated with another URI; however, future requests should still use the original URI.In contrast to how 302 was historically implemented, the request method is not allowed to be changed when reissuing the original request.For instance, a POST request should be repeated using another POST request.[32]
	Http_308_Permanent_Redirect = 308,//(RFC 7538) The request, and all future requests should be repeated using another URI. 307 and 308 (as proposed) parallel the behaviours of 302 and 301, but do not allow the HTTP method to change.So, for example, submitting a form to a permanently redirected resource may continue smoothly.
	Http_308_Resume_Incomplete = 308,//(Google) This code is used in the Resumable HTTP Requests Proposal to resume aborted PUT or POST requests
	//4 4xx Client Error
	//The 4xx class of status code is intended for cases in which the client seems to have erred.Except when responding to a HEAD request, the server should include an entity containing an explanation of the error situation, and whether it is a temporary or permanent condition.These status codes are applicable to any request method.User agents should display any included entity to the user.[35]
	Http_400_Bad_Request = 400,//The server cannot or will not process the request due to something that is perceived to be a client error(e.g., malformed request syntax, invalid request message framing, or deceptive request routing).
	Http_401_Unauthorized = 401,//(RFC 7235) Similar to 403 Forbidden, but specifically for use when authentication is required and has failed or has not yet been provided.The response must include a WWW-Authenticate header field containing a challenge applicable to the requested resource.See Basic access authentication and Digest access authentication.[37] 401 semantically means "unauthenticated", i.e. "you don't have necessary credentials".
	Http_402_Payment_Required = 402,//Reserved for future use.The original intention was that this code might be used as part of some form of digital cash or micropayment scheme, but that has not happened, and this code is not usually used.Google Developers API uses this status if a particular developer has exceeded the daily limit on requests.[38]
	Http_403_Forbidden = 403,//	The request was a valid request, but the server is refusing to respond to it.Unlike a 401 Unauthorized response, authenticating will make no difference.[39] 403 error semantically means "unauthorized", i.e. "you don't have necessary permissions for the resource".
	Http_404_Not_Found= 404,//The requested resource could not be found but may be available again in the future.Subsequent requests by the client are permissible.
	Http_405_Method_Not_Allowed = 405,//A request was made of a resource using a request method not supported by that resource; for example, using GET on a form which requires data to be presented via POST, or using PUT on a read-only resource.
	Http_406_Not_Acceptable = 406,//The requested resource is only capable of generating content not acceptable according to the Accept headers sent in the request.
	Http_407_Proxy_Authentication_Required = 407,//(RFC 7235) The client must first authenticate itself with the proxy.[42]
	Http_408_Request_Timeout = 407,//The server timed out waiting for the request.According to HTTP specifications : "The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time."[43]
	Http_409_Conflict = 409,//Indicates that the request could not be processed because of conflict in the request, such as an edit conflict in the case of multiple updates.[44]
	Http_410_Gone = 410,//Indicates that the resource requested is no longer available and will not be available again.This should be used when a resource has been intentionally removed and the resource should be purged.Upon receiving a 410 status code, the client should not request the resource again in the future.Clients such as search engines should remove the resource from their indices.[45] Most use cases do not require clients and search engines to purge the resource, and a "404 Not Found" may be used instead.
	Http_411_Length_Required = 411,//The request did not specify the length of its content, which is required by the requested resource.[46]
	Http_412_Precondition_Failed= 412,//(RFC 7232) The server does not meet one of the preconditions that the requester put on the request.[47]
	Http_413_Payload_Too_Large = 413,//(RFC 7231) The request is larger than the server is willing or able to process.Called "Request Entity Too Large" previously.[48]
	Http_414_URI_Too_Long = 414,//(RFC 7231) The URI provided was too long for the server to process.Often the result of too much data being encoded as a query-string of a GET request, in which case it should be converted to a POST request.[49] Called "Request-URI Too Long" previously.[50]
	Http_415_Unsupported_Media_Type = 415,//The request entity has a media type which the server or resource does not support.For example, the client uploads an image as image/svg+xml, but the server requires that images use a different format.
	Http_416_Range_Not_Satisfiable = 416,//(RFC 7233) The client has asked for a portion of the file(byte serving), but the server cannot supply that portion.For example, if the client asked for a part of the file that lies beyond the end of the file.[51] Called "Requested Range Not Satisfiable" previously.
	Http_417_Expectation_Failed = 417,//The server cannot meet the requirements of the Expect request-header field.[53]
	Http_418_Im_teapot = 418,//(RFC 2324) This code was defined in 1998 as one of the traditional IETF April Fools' jokes, in RFC 2324, Hyper Text Coffee Pot Control Protocol, and is not expected to be implemented by actual HTTP servers. The RFC specifies this code should be returned by tea pots requested to brew coffee. This HTTP status is used as an easter egg in some websites, including Google.com.
	Http_419_Authentication_Timeout = 419,//(not in RFC 2616) Not a part of the HTTP standard, 419 Authentication Timeout denotes that previously valid authentication has expired.It is used as an alternative to 401 Unauthorized in order to differentiate from otherwise authenticated clients being denied access to specific server resources.[citation needed][56]
	Http_420_Method_Failure = 420,//(Spring Framework) Not part of the HTTP standard, but defined by Spring in the HttpStatus class to be used when a method failed.This status code is deprecated by Spring.
	Http_420_Enhance_Your_Calm = 420,//(Twitter) Not part of the HTTP standard, but returned by version 1 of the Twitter Search and Trends API when the client is being rate limited. Other services may wish to implement the 429 Too Many Requests response code instead.
	Http_421_Misdirected_Request = 421,//(RFC 7540) The request was directed at a server that is not able to produce a response(for example because a connection reuse).
	Http_422_Unprocessable_Entity = 422,//(WebDAV; RFC 4918) The request was well-formed but was unable to be followed due to semantic errors.[17]
	Http_423_Locked = 423,//(WebDAV; RFC 4918) The resource that is being accessed is locked.
	Http_424_Failed_Dependency = 424,//(WebDAV; RFC 4918) The request failed due to failure of a previous request(e.g., a PROPPATCH).[17]
	Http_426_Upgrade_Required = 425,//The client should switch to a different protocol such as TLS/1.0, given in the Upgrade header field.
	Http_428_Precondition_Required = 428,//(RFC 6585) The origin server requires the request to be conditional.Intended to prevent "the 'lost update' problem, where a client GETs a resource's state, modifies it, and PUTs it back to the server, when meanwhile a third party has modified the state on the server, leading to a conflict."
	Http_429_Too_Many_Requests = 429,//(RFC 6585) The user has sent too many requests in a given amount of time.Intended for use with rate limiting schemes.
	Http_431_Request_Header_Fields_Too_Large = 431,//(RFC 6585) The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large.
	Http_440_Login_Timeout = 440,//(Microsoft) A Microsoft extension.Indicates that your session has expired.
	Http_444_No_Response = 444,//(Nginx) Used in Nginx logs to indicate that the server has returned no information to the client and closed the connection(useful as a deterrent for malware).
	Http_449_Retry_With = 449,//(Microsoft) A Microsoft extension.The request should be retried after performing the appropriate action.
	Http_450_Blocked_by_Windows_Parental_Controls = 450,//(Microsoft) 	A Microsoft extension.This error is given when Windows Parental Controls are turned on and are blocking access to the given webpage.[65]
	Http_451_Unavailable_For_Legal_Reasons = 451,//(Internet draft)	Defined in the internet draft "A New HTTP Status Code for Legally-restricted Resources".[66] Intended to be used when resource access is denied for legal reasons, e.g.censorship or government-mandated blocked access.A reference to the 1953 dystopian novel Fahrenheit 451, where books are outlawed.
	Http_451_Redirect = 451,//(Microsoft) Used in Exchange ActiveSync if there either is a more efficient server to use or the server cannot access the users' mailbox. The client is supposed to re-run the HTTP Autodiscovery protocol to find a better suited server.[69]
	Http_494_Request_Header_Too_Large = 494,//(Nginx) Nginx internal code similar to 431 but it was introduced earlier in version 0.9.4 (on January 21, 2011).[70][original research ? ]
	Http_495_Cert_Error = 495,//(Nginx) Nginx internal code used when SSL client certificate error occurred to distinguish it from 4XX in a log and an error page redirection.
	Http_496_No_Cert = 496,//(Nginx) Nginx internal code used when client didn't provide certificate to distinguish it from 4XX in a log and an error page redirection.
	Http_497_HTTP_to_HTTPS = 497,//(Nginx) Nginx internal code used for the plain HTTP requests that are sent to HTTPS port to distinguish it from 4XX in a log and an error page redirection.
	Http_498_Token_expired = 498,//or invalid (Esri) Returned by ArcGIS for Server.A code of 498 indicates an expired or otherwise invalid token.
	Http_499_Client_Closed_Request = 499,//(Nginx) Used in Nginx logs to indicate when the connection has been closed by client while the server is still processing its request, making server unable to send a status code back.[75]
	Http_499_Token_required = 499,//(Esri) Returned by ArcGIS for Server.A code of 499 indicates that a token is required(if no token was submitted).
	//5 5xx Server Error - the server failed to fulfill an apparently valid request.
	//Response status codes beginning with the digit "5" indicate cases in which the server is aware that it has encountered an error or is otherwise incapable of performing the request.Except when responding to a HEAD request, the server should include an entity containing an explanation of the error situation, and indicate whether it is a temporary or permanent condition.Likewise, user agents should display any included entity to the user.These response codes are applicable to any request method.[77]
	Http_500_Internal_Server_Error = 500,//A generic error message, given when an unexpected condition was encountered and no more specific message is suitable
	Http_501_Not_Implemented = 501,//The server either does not recognize the request method, or it lacks the ability to fulfill the request.Usually this implies future availability(e.g., a new feature of a web-service API).[79]
	Http_502_Bad_Gateway = 502,//The server was acting as a gateway or proxy and received an invalid response from the upstream server.
	Http_503_Service_Unavailable = 503,//The server is currently unavailable(because it is overloaded or down for maintenance).Generally, this is a temporary state.
	Http_504_Gateway_Timeout = 504,//The server was acting as a gateway or proxy and did not receive a timely response from the upstream server.
	Http_505_HTTP_Version_Not_Supported = 505,//The server does not support the HTTP protocol version used in the request.[83]
	Http_506_Variant_Also_Negotiates = 506,//(RFC 2295) Transparent content negotiation for the request results in a circular reference.
	Http_507_Insufficient_Storage = 507,//(WebDAV; RFC 4918) The server is unable to store the representation needed to complete the request.
	Http_508_Loop_Detected = 508,//(WebDAV; RFC 5842) The server detected an infinite loop while processing the request(sent in lieu of 208 Already Reported).
	Http_509_Bandwidth_Limit_Exceeded = 509,//(Apache bw/limited extension) This status code is not specified in any RFCs. Its use is unknown.
	Http_510_Not_Extended = 510,//(RFC 2774) Further extensions to the request are required for the server to fulfil it.[86]
	Http_511_Network_Authentication_Required = 511,//(RFC 6585) The client needs to authenticate to gain network access.Intended for use by intercepting proxies used to control access to the network(e.g., "captive portals" used to require agreement to Terms of Service before granting full Internet access via a Wi-Fi hotspot).
	Http_520_Unknown_Error = 520,//This status code is not specified in any RFC and is returned by certain services, for instance Microsoft Azure and CloudFlare servers : "The 520 error is essentially a “catch-all” response for when the origin server returns something unexpected or something that is not tolerated/interpreted (protocol violation or empty response)."[87]
	Http_522_Origin_Connection_Timeout = 522,//This status code is not specified in any RFCs, but is used by CloudFlare's reverse proxies to signal that a server connection timed out.
	Http_598_Network_read_timeout_error = 598,//(Unknown) This status code is not specified in any RFCs, but is used by Microsoft HTTP proxies to signal a network read timeout behind the proxy to a client in front of the proxy.[citation needed]
	Http_599_Network_connect_timeout_error = 599,//(Unknown) This status code is not specified in any RFCs, but is used by Microsoft HTTP proxies to signal a network connect timeout behind the proxy to a client in front of the proxy.
};
namespace Qqt {
	QString toString(HttpStatusCode code);
}