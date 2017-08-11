package bgu.spl171.net.api.bidi;
import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;
import bgu.spl171.net.srv.ConnectionHandler;

public class ConnectionsImpl<T> implements Connections<T> {
	private ConcurrentHashMap<Integer, ConnectionHandler<T>> _connectionHandlerList = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
    private ArrayList<String> _usersNames = new ArrayList<String>();
    private ArrayList<String> _filesNames = new ArrayList<String>();
	
	public boolean send(int connectionId, T msg){
		_connectionHandlerList.get(connectionId).send(msg);
		return true;
	}

	public void broadcast(T msg){
		for(Integer currentId : _connectionHandlerList.keySet())
			_connectionHandlerList.get(currentId).send(msg);
	}

	public void disconnect(int connectionId){
		_connectionHandlerList.remove(connectionId);
	}

	public void createConnectionIdForHandler(ConnectionHandler<T> handler, int connectionId){
		_connectionHandlerList.putIfAbsent(connectionId, handler);
	}
	
	public ArrayList<String> getUsersNames(){
		return _usersNames;
	}
	
	public ArrayList<String> getFilesNames(){
		return _filesNames;
	}
}
