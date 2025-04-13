module Main (main) where


import LogLib.Logger (msglog)
import Network.Socket
import Network.Socket.ByteString (sendAll)
import qualified Data.ByteString.Char8 as BS
import System.Environment (lookupEnv)

main :: IO ()
main = do
  msglog ("Haskell Logger Enabled")
  msglog ("Haskell Logger Still Enabled")
  frontendSocketPath <- lookupEnv "FRONTEND_SOCKET_PATH"
  case frontendSocketPath of
    Nothing -> putStrLn "Error: FRONTEND_SOCKET_PATH not set"
    Just socketPath -> do
      sock <- socket AF_UNIX Stream 0
      connect sock (SockAddrUnix socketPath)
      sendAll sock (BS.pack "Hello from Haskell")
      close sock


{-
import Control.Monad (forever)

main :: IO()
main = forever (return ())
-}