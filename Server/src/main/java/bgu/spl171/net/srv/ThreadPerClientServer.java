package bgu.spl171.net.srv;

import java.util.function.Supplier;

import bgu.spl171.net.api.MessageEncoderDecoder;
import bgu.spl171.net.api.bidi.BidiMessagingProtocol;

public class ThreadPerClientServer extends BaseServer<Packet> {
	
	public ThreadPerClientServer(
            int port,
            Supplier<BidiMessagingProtocol<Packet>> protocolFactory,
            Supplier<MessageEncoderDecoder<Packet>> encdecFactory) {

		super(port, protocolFactory, encdecFactory);
	}

	@Override
	protected void execute(BlockingConnectionHandler<Packet> handler) {
		new Thread(handler).start();
	}
	

}
