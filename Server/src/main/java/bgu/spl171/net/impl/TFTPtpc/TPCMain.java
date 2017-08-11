package bgu.spl171.net.impl.TFTPtpc;
import bgu.spl171.net.api.bidi.TFTProtocol;
import bgu.spl171.net.srv.PacketMessageEncoderDecoder;
import bgu.spl171.net.srv.ThreadPerClientServer;

public class TPCMain {
	public static void main(String[] args){  
        new ThreadPerClientServer(
                7777, //port
                () -> new TFTProtocol(), //protocol factory
                () -> new PacketMessageEncoderDecoder() //message encoder decoder factory
        ).serve();
	}
}
