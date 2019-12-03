package jp.pioneer.ceam.thirdpartyadapter.urlhttp;

import java.io.InputStream;

/**
 * 回复封装类
 */
public class RealResponse {
    public InputStream inputStream;
    public InputStream errorStream;
    public int code;
    public long contentLength;
    public Exception exception;
}
