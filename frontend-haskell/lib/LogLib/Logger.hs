module LogLib.Logger (msglog) where

import System.Environment (lookupEnv)
import Network.Socket
import Network.Socket.ByteString (sendAll)
import qualified Data.ByteString.Char8 as BS


msglog :: String -> IO ()
msglog stringToLog = do
    logEnabled <- lookupEnv "PYROXENE_LOG"
    pyroxLogSocketPath <- lookupEnv "PYROXENE_LOG_SOCKET_PATH"
    case logEnabled of
        Just "1" -> 
            case pyroxLogSocketPath of
                Nothing -> putStrLn "Error: PYROXENE_LOG_SOCKET_PATH not set"
                Just socketPath -> do
                    sock <- socket AF_UNIX Stream 0
                    connect sock (SockAddrUnix socketPath)
                    sendAll sock (BS.pack stringToLog)
                    close sock
        _   -> return ()
