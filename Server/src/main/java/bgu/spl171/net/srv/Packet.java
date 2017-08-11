package bgu.spl171.net.srv;

public abstract class Packet {
	public abstract short getOpcode();
	public abstract String getString();
}
