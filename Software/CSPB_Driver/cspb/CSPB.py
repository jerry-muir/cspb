# Copyright 2020 Gerard L. Muir 
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and or sell
#  copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE.

import smbus
import time


class CSPB:
    """ 
    A class for i2c communication with the cluster system power board.
    
    Attributes
    ----------
    i2c_port: int
        the i2c bus or port number.    
    address: int
        the i2c bus address of the cluster system power board.
    
    Methods
    -------
    set_power(value):
        Sends the power command value to the cluster system power board.
        
    shutdown(value):
        Sends the shutdown command value to the cluster system power board.
        
    signal_shutdown(value):
        Sends the shutdown signal only command value to the cluster system 
        power board.
        
    read_register(register_number):
        returns the data for the specified register.
        
    write_register(register_number, value):
        writes the given data to the specified register.
    
    set_register_number(register_number):
        sets the register number for the read_register command.
    
    send_command(command):
        sends the command string over i2c to the cluster system power board.
    
    Author: Gerard L. Muir
    """
    
    # Command character definitions.
    POWER = 0x21 # hex value for ! character
    SLOT_SHUTDOWN = 0x23 # hex value for # character
    SIGNAL_SHUTDOWN = 0x24 # hex value for $ character
    SET_REGISTER = 0x52 # hex value for R character
    WRITE_REGISTER = 0x57 # hex value for W character
    
    DEVICE_REG_MODE1 = 0x00
    
    # CSPB EEPROM register addresses.
    I2C_ADDR_RGSTR_ADDR = 0  # i2c boot up address
    SLT_PWR_UP_RGSTR_ADDR = 1  # slot power up on boot
    APP_DATA_RGSTR_ADDR = 2  # application data
    PWR_UP_DELAY = 3  # slot power up delay
    PWR_DWN_SGNL_DRTN_RGSTR_ADDR = 4  # power down signal duration
    SHTDWN_TIME_OUT_RGSTR_ADDR = 5  # slot shutdown timeout 
    MNTR_LN_STTS_RGSTR_ADDR = 128 # slot monitor line status 
    PWR_STTS_RGSTR_ADDR = 129 # slot power status
    IN_SHUTDOWN_RGSTR_ADDR = 130 # Slot(s) in shutdown status
    BUS_SETTLING_TIME = .005 # delay time to let i2c bus become available
                             # between commands.


    def __init__(self, i2c_port, address):
        """
        Construct all necessary attributes for the CSPB object.
    
        Attributes
        ----------
        i2c_port: int
            the i2c bus or port number.    
        address: int
            the i2c bus address of the cluster system power board.
        """
        
        self.i2c_port = i2c_port # 0 = /dev/i2c-0 (port I2C0),
                                 # 1 = /dev/i2c-1 (port I2C1)
        self.address = address   # i2c buss address in hex form.
        self.bus = smbus.SMBus(i2c_port)
 
        
    def set_power(self, value):
        """
        Sends the power command to the cluster system power board slots.
        
        The power register is a 4 bit value representing the power slots with
        the least significant bit representing the first power slot. 
        
        Parameters
        ----------
        value : int (0-15)
            the power register value to write.

        Returns
        -------
        None

        """
        
        cspb_command = [self.POWER, value]
        self.send_command(cspb_command)
   
        
    def shutdown(self, value):
        """
        Sends the shutdown command to the cluster system power board slots.
        
        The shutdown register is a 4 bit value representing the power slots with
        the least significant bit representing the first power slot. 
        
        Parameters
        ----------
        value : int (0-15)
            the shutdown command value to write.

        Returns
        -------
        None

        """
        
        cspb_command = [self.SLOT_SHUTDOWN, value]
        self.send_command(cspb_command)
      
        
    def signal_shutdown(self, value):
        """
        Sends the shutdown signal only command to the cluster system power
        board slots.
        
        The shutdown signal only register is a 4 bit value representing the 
        power slots with the least significant bit representing the first power
        slot. 
        
        Parameters
        ----------
        value : int (0-15)
            the signal shutdown command value to write.

        Returns
        -------
        None

        """
        
        cspb_command = [self.SIGNAL_SHUTDOWN, value] 
        self.send_command(cspb_command)
      
            
    def read_register(self, register_number):
        """
        Sends the read register command to the cluster system power board.
        
        Parameters
        ----------
        register_number : int
            the register to read.

        Returns
        -------
        the value of the specified register.

        """
        
        self.set_register_number(register_number)
        time.sleep(self.BUS_SETTLING_TIME)
        return (self.bus.read_byte(self.address))
    
    
    def write_register(self, register_number, value):
        """
        Sends the write register command to the cluster system power board.
        
        Parameters
        ----------
        register_number : int
            the register to be written.
        value : int
            the value to be written. (0-255)

        Returns
        -------
        None
        """
        
        cspb_command = [self.WRITE_REGISTER, register_number, value] 
        self.send_command(cspb_command)
      
             
    def set_register_number(self, register_number):
        """
        Sends the set register command to the cluster system power board.
        
        Parameters
        ----------
        register_number : int
            the register to be read by the read register command.

        Returns
        -------
        None

        """
        
        cspb_command = [self.SET_REGISTER, register_number] 
        self.send_command(cspb_command)
      
             
    def send_command(self, command):
        """
        Sends the specified command string over the i2c bus to the cluster
        system power board.
        
        Parameters
        ----------
        command : string
            the command string to be sent.

        Returns
        -------
        None

        """
        self.bus.write_i2c_block_data(self.address, self.DEVICE_REG_MODE1, command)
      
        
# Print a test value if this class file is run by itself. This is a
# small self test routine.
if __name__ == '__main__':
    cspb = CSPB(1, 0x00)
    print(cspb.address)