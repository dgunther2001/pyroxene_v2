module LogLib.Logger (msglog) where

import System.Environment (lookupEnv)
import Data.Time.LocalTime (getZonedTime)
import Data.Time.Format (formatTime, defaultTimeLocale)

msglog :: String -> IO ()
msglog stringToLog = do
    logEnabled <- lookupEnv "PYROXENE_LOG"
    pyroxLogPath  <- lookupEnv "PYROXENE_LOG_PATH"
    case logEnabled of
        Just "1" -> 
            case pyroxLogPath of
                Just path -> do
                    timeAsString <- getCurrentTime
                    appendFile path timeAsString
                    appendFile path " "
                    appendFile path stringToLog
                    appendFile path "\n"
                _ -> return ()
        _   -> return ()


getCurrentTime :: IO String
getCurrentTime = do 
    time <- getZonedTime
    return $ formatTime defaultTimeLocale "%T" time
