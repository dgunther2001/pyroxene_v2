module Main (main) where

import LogLib.Logger (msglog)
import Network.Socket
import Network.Socket.ByteString (sendAll)
import qualified Data.ByteString.Char8 as BS

socketPath :: FilePath
socketPath = "../tmp/frontend_pyroxene.sock"

main :: IO ()
main = do
  msglog ("Haskell Logger Enabled")
  msglog ("Haskell Logger Still Enabled")
  sock <- socket AF_UNIX Stream 0
  connect sock (SockAddrUnix socketPath)
  sendAll sock (BS.pack "Hello from Haskell")
  close sock