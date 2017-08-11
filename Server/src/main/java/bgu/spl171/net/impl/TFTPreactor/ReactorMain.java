package bgu.spl171.net.impl.TFTPreactor;

import bgu.spl171.net.api.bidi.TFTProtocol;
import bgu.spl171.net.srv.PacketMessageEncoderDecoder;
import bgu.spl171.net.srv.Server;

public class ReactorMain {
	public static void main(String[] args){  
        Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                7777, //port
                () -> new TFTProtocol(), //protocol factory
                () -> new PacketMessageEncoderDecoder() //message encoder decoder factory
        ).serve();
	}
}
