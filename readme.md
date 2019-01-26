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
<img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/wiring/HomeyLedDimmer_v1-0_bb.png" width="500">

## PCB Layout
<img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/HomeyLedDimmer_v1-0_pcb.jpg" width="195"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20board.JPG" width="200"> <img src="https://github.com/MagnusPer/HomeyLedDimmer/blob/master/extras/PCB%20layout/PCB%20mounted.JPG" width="209">


## BOM List
| Part                               | Link                                                |
| ---------------------------------- | --------------------------------------------------- |
|  Terminal block                    |                                                     |      
|  MOSFET                            |                                                     |
|  Wemos D1 mini                     |                                                     |
|  Capacitor                             |                                                     |
|  MOSFET                            |                                                     |
|  MOSFET                            |                                                     |
|  MOSFET                            |                                                     |
|  MOSFET                            |                                                     |

## Fade curve


## References
- Athom Homey: https://www.athom.com/en/
- Wemos D1 Mini: https://wiki.wemos.cc/products:d1:d1_mini



## How to use

See [example](examples/DimmerControl/DimmerControl)
