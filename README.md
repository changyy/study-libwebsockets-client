# platform: macOS

## libwebsockets

```
% port search libwebsockets 
libwebsockets @4.3.2 (devel, net)
    Canonical libwebsockets.org websocket library
% sudo port install libwebsockets
```

## build code

```
% pkg-config libwebsockets --libs --cflags
-I/opt/local/include -L/opt/local/lib -lwebsockets
```

```
% make
gcc -g -Wall main.c `pkg-config libwebsockets --libs --cflags`
% file a.out
a.out: Mach-O 64-bit executable x86_64
% ./a.out
Usage> ./a.out url
	 ./a.out ws://localhost:8000/
	 ./a.out wss://ws.ptt.cc/bbs app://cmd
	 ./a.out wss://ws.ptt.cc/bbs https://term.ptt.cc
```

## run code

```
% ./a.out wss://ws.ptt.cc/bbs https://term.ptt.cc
[2022/08/25 20:39:08:1699] N: lws_create_context: LWS: 4.3.2-no_hash, NET CLI SRV H1 H2 WS ConMon IPV6-on
[2022/08/25 20:39:08:1705] N: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2022/08/25 20:39:08:1724] N: __lws_lc_tag:  ++ [vh|0|default||-1] (1)
wsCallback - LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS
[INFO] libwebsockets version: 4.3.2
[INFO] lws_parse_uri - Server: ws.ptt.cc:443
[INFO] lws_parse_uri - URLScheme: wss
[INFO] lws_parse_uri - URLPath: bbs
[INFO] clientConnectInfo.addres: ws.ptt.cc
[INFO] clientConnectInfo.port: 443
[INFO] clientConnectInfo.path: /bbs
[INFO] clientConnectInfo.host: ws.ptt.cc
[INFO] clientConnectInfo.origin: https://term.ptt.cc
[INFO] clientConnectInfo.ssl_connection: 109
[2022/08/25 20:39:08:2205] N: __lws_lc_tag:  ++ [wsicli|0|WS/h1/default/ws.ptt.cc] (1)
call lws_client_connect_via_info done, webSocket is NULL(NO)
[2022/08/25 20:39:08:2409] N: lws_gate_accepts: on = 0
[2022/08/25 20:39:08:2547] N: lws_gate_accepts: on = 0
wsCallback - LWS_CALLBACK_CLIENT_ESTABLISHED
wsCallback - LWS_CALLBACK_CLIENT_WRITEABLE
wsCallback - LWS_CALLBACK_CLIENT_RECEIVE
RECEIVER: HTTP/1.1 200 OK

     ??      PTT                ?P   ??        ?P  ??      ???i?i?i?i?i?i??
             140.112.172.11 ?P     ???i???i??            ???i?i?i?i?i
  ?z?w?{     ?????~?{        ???d?i?i???i??   ?P   ???i?i?i?i?i??  ?P
  ?x?V?|?{   ptt.cc            ???i?i?i?i?i???i??    ???i?i?i?i?i
  ?x?V  ?x                   ???i?i?i???i?i?i????  ???i?i?i?i?i??  ??  ?P
?w?}    ?x?z?w?w?{  ?P       ????      ?i?i?i?i?????i?i?i?i    ?P
        ?|?t  ?V?x      ?P           ???i?i?i?i???i?i?i??            ?P
                ?x?z?w?w?w?{         ?i?i?i?i?i?h?h?g?g?f?f?e?e?d?c?b
            ?z?w?r?}?V?V  ?| ???i?i?i?i?i?h?h?g?g?f?f?e?e?d?d?c?c?b
            ?x?V?V           ???i?i
wsCallback - LWS_CALLBACK_CLIENT_RECEIVE
?п?J?N???A?ΥH guest ???[?A?ΥH new ???U:               
^C
```

---

# platform: Linux

## libwebsockets

```
$ cd libwebsockets
$ mkdir build 
$ cd build
$ cmake .. -DLWS_WITHOUT_EXTENSIONS=OFF
$ make
```

## build code

```
$ gcc -g -Wall main.c -I libwebsockets/build/include/ -L g/libwebsockets/build/lib -l websockets
```

## run  code

```
$ LD_LIBRARY_PATH=libwebsockets/build/lib:$LD_LIBRARY_PATH ./a.out  wss://ws.ptt.cc/bbs app://cmd
```
