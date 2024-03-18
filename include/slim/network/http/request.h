#ifndef __SLIM__NETWORK__HTTP__REQUEST__H
#define __SLIM__NETWORK__HTTP__REQUEST__H
#include <iostream>
#include <unordered_map>
#include <variant>
#include <slim/network/http/headers.h>
//https://fetch.spec.whatwg.org/
//https://fetch.spec.whatwg.org/#request-class
namespace slim::network::http {
	enum ReferrerPolicy {"","no-referrer","no-referrer-when-downgrade","same-origin",
						"origin","strict-origin","origin-when-cross-origin",
  						"strict-origin-when-cross-origin","unsafe-url" };
	enum RequestDestination { "", "audio", "audioworklet", "document", "embed", "font",
							"frame", "iframe", "image", "json", "manifest", "object", "paintworklet",
							"report", "script", "sharedworker", "style",  "track", "video", "worker", "xslt" };
	enum RequestMode { "navigate", "same-origin", "no-cors", "cors" };
	enum RequestCredentials { "omit", "same-origin", "include" };
	enum RequestCache { "default", "no-store", "reload", "no-cache", "force-cache", "only-if-cached" };
	enum RequestRedirect { "follow", "error", "manual" };
	enum RequestDuplex { "half" };
	enum RequestPriority { "high", "low", "auto" };

/* interface mixin Body {
  readonly attribute ReadableStream? body;
  readonly attribute boolean bodyUsed;
  [NewObject] Promise<ArrayBuffer> arrayBuffer();
  [NewObject] Promise<Blob> blob();
  [NewObject] Promise<FormData> formData();
  [NewObject] Promise<any> json();
  [NewObject] Promise<USVString> text();
}; */

	//typedef (Request or USVString) RequestInfo;
	typedef std::variant<Request, std::string> RequestInfo;
	typedef std::string DOMString;
	struct RequestInit {
  		std::string method = "GET";
  		slim::network::http::HeadersInit headers;
  //BodyInit? body; //https://fetch.spec.whatwg.org/#bodyinit
  		std::string referrer;
  		ReferrerPolicy referrerPolicy;
  		RequestMode mode;
  		RequestCredentials credentials;
  		RequestCache cache;
  		RequestRedirect redirect;
  		DOMString integrity;
  		bool keepalive;
//https://dom.spec.whatwg.org/#abortsignal
  //AbortSignal? signal;
  		RequestDuplex duplex = "half";
  		RequestPriority priority;
  		//any window; // can only be set to null
	};
	class Request {
			std::string method = "GET";
			std::string url;
			slim::network::http::Headers headers;
			RequestDestination destination;
 			std::string referrer;
			ReferrerPolicy referrerPolicy;
			RequestMode mode;
			RequestCredentials credentials;
			RequestCache cache;
			RequestRedirect redirect;
			DOMString integrity;
			bool keepalive;
			bool isReloadNavigation;
			bool isHistoryNavigation;
//https://dom.spec.whatwg.org/#abortsignal
			//AbortSignal signal;
			RequestDuplex duplex = "half";
		public:
			Request(const RequestInfo requestInfo, const RequestInit requestInit = {});
			clone(void);
	};
}
#endif