package example;

import ssobjects.*;

public class User {
    public TelnetServerSocket sock;
    public UserStateEnum state;
    public String username;
    public String password;
    public String passwordVerify;
    public long lastActivity;       //when last command from user came (keep track of how long user has been idle

    public User(TelnetServerSocket s) {
        this(s,"");
    }
    public User(TelnetServerSocket s,String u) {
        sock = s;
        username = u;
        state=UserStateEnum.LOGIN;
        updateIdle();
    }

    public void updateIdle() {
        lastActivity = System.currentTimeMillis();
    }

    public long idleTime() {
        return System.currentTimeMillis()-lastActivity;
    }
}
