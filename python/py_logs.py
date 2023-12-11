
import logging
from colorlog import ColoredFormatter


class LOGGING :
    def __init__(self,name) -> None:
        self.logger = logging.getLogger(name=name)
        # set the coloredformatter 
        self.formatter = ColoredFormatter(
            fmt= "%(log_color)s%(levelname)-4s %(message)s %(reset)s",
                datefmt=None,
                reset=True,
                log_colors={
                    'DEBUG': 'cyan',
                    'INFO': 'green',
                    'WARNING': 'yellow',
                    'ERROR': 'red',
                    'CRITICAL': 'red,bg_white',
                },
                secondary_log_colors={},
                style='%'
                )
        
        # Create a console handler and set the formatter
        self.ch = logging.StreamHandler()
        self.ch.setFormatter(self.formatter)

        # Add the console handler to the logger
        self.logger.addHandler(self.ch)

        # Set the logging level (optional)
        self.logger.setLevel(logging.DEBUG)
        
    def __del__(self) ->None:
        pass 
    
    def debug(self,msg):
        self.logger.debug(msg)
        
    def info(self,msg):
        self.logger.info(msg)
        
    def warning(self,msg):
        self.logger.warning(msg)
        
    def error(self,msg):
        self.logger.error(msg)
    
    def critical(self,msg):
        self.logger.critical(msg)

