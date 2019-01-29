# Homeyduino LED Dimmer
An [Wemos D1 Mini](https://wiki.wemos.cc/products:d1:d1_mini) based 1-channel LED PWM Dimmer controled by [Homey](https://www.athom.com/en/) home automation system and/or by a physical button. 

The dimmer control function is based the [DimmerControl](https://github.com/Adminius/DimmerControl) library. 


## Installation
In the Homey install the [Homeyduino](https://apps.athom.com/app/com.athom.homeyduino) app 

Needed Arduino Libraries to be included in [IDE](https://www.arduino.cc/en/Main/Software). Install them either from GitHub repository directly or within the IDE application itself **Sketch > Import Library** 

| Library                            | Link to GitHub                                      |
| ---------------------------------- | --------------------------------------------------- |
| Homeyduino                         |  https://github.com/athombv/homey-arduino-library   |      
| DimmerControl  v1.0.0              |  https://github.com/Adminius/DimmerControl          |


## Wiring
<img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/wiring/HomeyLedDimmer_v1-0.png" width="400">

## PCB Layout
Gerber files for production is located in the [extras/PCB Layout/Gerber](https://github.com/MagnusPer/Homeyduino_LedDimmer/tree/master/extras/PCB%20layout/Gerber) folder.

<img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/HomeyLedDimmer_v1-0_pcb.jpg" width="185"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20top.JPG" width="180"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20mounted%20top.JPG" width="220"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20mounted%20side.JPG" width="230">


## BOM List
| Part                               | Comment/Link                                        |
| ---------------------------------- | --------------------------------------------------- |
|  Wemos D1 mini                     |                                                     |   
|  Terminal block - 5.08mm           | https://www.rutronik24.com/product/sauro/msb02005/5511.html |      
|  MOSFET - IRLZ44N                  | http://www.irf.com/product-info/datasheets/data/irlz44n.pdf |
|  Voltage Regulator - LD1117v33     | https://www.sparkfun.com/datasheets/Components/LD1117V33.pdf|
|  Capacitor 10uF                    |                                                     |
|  Capacitor 100nF                   |                                                     |
|  Resistor 1 kohm                   |                                                     |
|  Resistor 220 kohm                 |                                                     |

## Features
 - Customized linear fade curve
 - OTA

## References
- Athom Homey: https://www.athom.com/en/
- Wemos D1 Mini: https://wiki.wemos.cc/products:d1:d1_mini

## How to use Dimmer Control Library 
Visit the [Dimmer Control](https://github.com/Adminius/DimmerControl) by Adminius repository.
