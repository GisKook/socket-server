socket-server : echosrv.c socket_server.c cetcnav_dasserver.cpp kfifo.c cetcnav_dasserver.cpp CNProtocolPersonPos.cpp CNToolkit.cpp mdp_client.c CNBaseInfo.pb.cc CNReportControl.pb.cc CNReportLocation.pb.cc
	g++ -g -Wall -I ./pb -o $@ $^ -lpthread -lczmq -lzmq -lprotobuf

clean:
	rm socket-server
