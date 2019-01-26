# HomeyLedDimmer
An [Wemos D1 Mini](https://wiki.wemos.cc/products:d1:d1_mini) based 1-channel LED PWM Dimmer controled by [Homey](https://www.athom.com/en/) home automation system and/or by a physical momentary switch. 

## Installation
In the Homey install the [Homeyduino](https://apps.athom.com/app/com.athom.homeyduino) app 

Needed Arduino Libraries to be included in [IDE](https://www.arduino.cc/en/Main/Software). Install them either from GitHub repositories directly or within the IDE application itself **Sketch > Import Library** 

| Library                            | Link to GitHub                                      |
| ---------------------------------- | --------------------------------------------------- |
| Homeyduino                         |  https://github.com/athombv/homey-arduino-library   |      
| DimmerControl                      |  https://github.com/Adminius/DimmerControl          |


## Wiring
<img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/wiring/HomeyLedDimmer_v1-0.png" width="400">

## PCB Layout
<img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/HomeyLedDimmer_v1-0_pcb.jpg" width="195"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20top.JPG" width="200"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20mounted%20top.JPG" width="209"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20mounted%20side.JPG" width="209">



## BOM List
| Part                               | Comment/Link                                        |
| ---------------------------------- | --------------------------------------------------- |
|  Wemos D1 mini                     |                                                     |   
|  Terminal block                    | 5.08 mm                                             |      
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



## How to use Dimmer Control library 
See [examples](https://github.com/Adminius/DimmerControl/tree/master/examples)
