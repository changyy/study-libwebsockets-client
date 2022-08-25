#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PAYLOAD_MAX_LENGTH (1024)
#define PATH_MAX_LENGTH (1024)

static struct lws *webSocket = NULL;

static int wsCallback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch(reason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
		{
			printf("wsCallback - LWS_CALLBACK_CLIENT_ESTABLISHED\n");
			lws_callback_on_writable(wsi);
			break;
		}
		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			printf("wsCallback - LWS_CALLBACK_CLIENT_RECEIVE\n");
			printf("RECEIVER: %s\n", (char *)in);
			break;
		}
		
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			printf("wsCallback - LWS_CALLBACK_CLIENT_WRITEABLE\n");
			break;
		}

		case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
		{
			printf("wsCallback - LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS\n");
			break;
		}

		case LWS_CALLBACK_CLOSED:
		{
			printf("wsCallback - LWS_CALLBACK_CLOSED\n");
			webSocket = NULL;
			break;
		}
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		{
			printf("wsCallback - LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
			webSocket = NULL;
			break;
		}
		default:
			break;
	}

	return 0;
}

void usage(int argc, char **argv) {
	printf("Usage> %s url\n", argv[0]);
	printf("\t %s ws://localhost:8000/\n", argv[0]);
	printf("\t %s wss://ws.ptt.cc/bbs app://cmd\n", argv[0]);
	printf("\t %s wss://ws.ptt.cc/bbs https://term.ptt.cc\n", argv[0]);
}

int main(int argc, char **argv) {
	char *input_url = NULL, *input_host = NULL, *input_origin = NULL;
	if (argc < 2) {
		usage(argc, argv);
		return 0;
	}
	input_url = argv[1];
	if (argc >= 3) {
		input_origin = argv[2];
	}
	if (argc >= 4) {
		input_host = argv[3];
	}

	// https://libwebsockets.org/lws-api-doc-master/html/md_README.coding.html
	// https://libwebsockets.org/lws-api-doc-master/html/group__log.html#ga36dda7d78375812fb9fbbcbc7d4ed52b
	//lws_set_log_level(5,NULL);
	//lwsl_visible(LLL_ERR|LLL_WARN|LLL_NOTICE|LLL_INFO|LLL_DEBUG|LLL_PARSER|LLL_HEADER|LLL_EXT|LLL_CLIENT|LLL_LATENCY|LLL_USER|LLL_THREAD|LLL_COUNT|LLLF_SECRECY_PII|LLLF_SECRECY_BEARER |LLLF_LOG_TIMESTAMP|LLLF_LOG_CONTEXT_AWARE );

	// https://libwebsockets.org/lws-api-doc-main/html/structlws__client__connect__info.html#aba35adfb74845a5fd0c3dc141cbdddd2
	// https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_client_applications
	struct lws_context *context;
	struct lws_context_creation_info contextCreationInfo = {0};
	struct lws_client_connect_info clientConnectInfo = {0};
	const char *wsURLScheme, *wsURLPath;
	int n;
	char path[PATH_MAX_LENGTH + 1] = {0};

	if (0 != lws_parse_uri(input_url, &wsURLScheme, &clientConnectInfo.address, &clientConnectInfo.port, &wsURLPath)) {
		printf("[ERRPR] lws_parse_uri\n");
		return 0;
	}

	path[0] = '/';
	if (strlen(wsURLPath) >= 1 && wsURLPath[0] != '/') {
		n = snprintf(path + 1, PATH_MAX_LENGTH, "%s", wsURLPath);
		path[1+n] = '\0';
		path[PATH_MAX_LENGTH] = '\0';
	}

	if (!strcmp("https", wsURLScheme) || !strcmp("wss", wsURLScheme)) {
		// https://libwebsockets.org/lws-api-doc-v2.1-stable/html/group__client.html
		//clientConnectInfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK | LCCSCF_ALLOW_INSECURE;

#ifdef LCCSCF_USE_SSL
		clientConnectInfo.ssl_connection |= LCCSCF_USE_SSL;
#else

#ifdef BIO_C_SET_SSL
		clientConnectInfo.ssl_connection |= BIO_C_SET_SSL;
#endif

#endif

#ifdef LCCSCF_ALLOW_SELFSIGNED
		clientConnectInfo.ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;
#endif

#ifdef LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK
		clientConnectInfo.ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
#endif

#ifdef LCCSCF_ALLOW_INSECURE
		clientConnectInfo.ssl_connection |= LCCSCF_ALLOW_INSECURE;
#endif
		//
		// E: SSL_new failed: error:00000063:lib(0)::reason(99)
		//
		contextCreationInfo.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

		// W: [wsicli|0|WS/h1/default/ws.ptt.cc]: lws_client_ws_upgrade: got bad HTTP response '403'
		// https://libwebsockets.org/lws-api-doc-main/html/group__context-and-vhost.html#ga54b2ce5fe0b87be6e14e12849c220319
		contextCreationInfo.options |= LWS_SERVER_OPTION_JUST_USE_RAW_ORIGIN;

		// macOS
		//contextCreationInfo.ssl_ca_filepath = "/usr/local/etc/openssl@1.1/cert.pem";
	}
	clientConnectInfo.protocol = "defaultWebSocket";
	//clientConnectInfo.ietf_version_or_minus_one = -1;

	// https://libwebsockets.org/libwebsockets-api-doc.html
	contextCreationInfo.port = CONTEXT_PORT_NO_LISTEN;
	contextCreationInfo.gid = -1;
	contextCreationInfo.uid = -1;

	//static const struct lws_extension exts[] = {
	//	{ "permessage-deflate", lws_extension_callback_pm_deflate, "permessage-deflate; client_max_window_bits" },   
	//	{ "deflate-frame", lws_extension_callback_pm_deflate, "deflate_frame" },
	//	{ NULL, NULL, NULL /* terminator */ }
	//};
	//clientConnectInfo.client_exts = exts;

	struct lws_protocols protocols[] = {
		{ "defaultWebSocket", wsCallback, 0, PAYLOAD_MAX_LENGTH}, 
#ifdef LWS_PROTOCOL_LIST_TERM
		LWS_PROTOCOL_LIST_TERM
#else
        { NULL, NULL, 0, 0}
#endif
	};
	contextCreationInfo.protocols = protocols;

	if (NULL == (context = lws_create_context(&contextCreationInfo))) {
		printf("[ERRPR] lws_create_context\n");
		return 0;
	}
	clientConnectInfo.context = context;

	//
	// E: lws_ssl_client_bio_create: Unable to get hostname
	//
	//clientConnectInfo.host = lws_canonical_hostname(context);
	clientConnectInfo.host = input_host == NULL ? clientConnectInfo.address : input_host ;

	clientConnectInfo.origin = input_origin == NULL ? clientConnectInfo.host : input_origin ;
	clientConnectInfo.path = path;

	printf("[INFO] libwebsockets version: %d.%d.%d\n", LWS_LIBRARY_VERSION_MAJOR, LWS_LIBRARY_VERSION_MINOR, LWS_LIBRARY_VERSION_PATCH);
	printf("[INFO] lws_parse_uri - Server: %s:%d\n", clientConnectInfo.address, clientConnectInfo.port);
	printf("[INFO] lws_parse_uri - URLScheme: %s\n", wsURLScheme);
	printf("[INFO] lws_parse_uri - URLPath: %s\n", wsURLPath);
	printf("[INFO] clientConnectInfo.addres: %s\n", clientConnectInfo.address);
	printf("[INFO] clientConnectInfo.port: %d\n", clientConnectInfo.port);
	printf("[INFO] clientConnectInfo.path: %s\n", clientConnectInfo.path);
	printf("[INFO] clientConnectInfo.host: %s\n", clientConnectInfo.host);
	printf("[INFO] clientConnectInfo.origin: %s\n", clientConnectInfo.origin);
	printf("[INFO] clientConnectInfo.ssl_connection: %d\n", clientConnectInfo.ssl_connection);

	time_t old = 0;
	while (1) {
		struct timeval tv;
		gettimeofday( &tv, NULL );

		if (tv.tv_sec != old) {
			if( webSocket == NULL ) {
				webSocket = lws_client_connect_via_info(&clientConnectInfo);
				printf("call lws_client_connect_via_info done, webSocket is NULL(%s)\n", (webSocket == NULL ? "YES" : "NO"));
			} else {
				printf("noop\n");
			}
			old = tv.tv_sec;
		}

		// https://libwebsockets.org/lws-api-doc-main/html/group__service.html#gaf95bd0c663d6516a0c80047d9b1167a8
		lws_service(context, 100);
	}

	lws_context_destroy(context);

	return 0;
}
