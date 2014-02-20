CSNetSim
========

A light simulator for wireless sensor network. It consists a small but complete simulator written in C++ and a MatLab interface.  
### Components  
This light-weighted simulator is composed of eight base components (Channel, Network, Clock, CommunicationProxy, Message, Node, Processor and Monitor) and some auxiliary components (Timer, Adjacency List of the network ...). All components are connected via flexible virtual interfaces, which makes them scalable and extendable.  
### Network
A simulator normally runs one network.  
A network is composed of one or more channels, a bunch of nodes, a clock and a monitor.  
### Channels  
Channels are at the bottom of the model and respond to transmit different kinds of messages between nodes. Broadcast-channel and unicast-channel are provided as standard channels, and more functional channel can be easily derived from pure virtual Channel. 
### Node  
A node generally has one communication-proxy and several processors.  
### Communication Proxy
Communication-proxy can be called to send and receive messages.  
### Processor
Processors processe different kind of messages and runs their own programs parallelly.   
Iterators and manager are provided to manage channels in network, messages in communication-proxy' buffer and processors in node, which make it easy to scale-out these components.  
### Clock  
Clock provides global time and its simulating step is variable, which enables a refined and efficient simulation.  
### Monitor  
Monitor is applied to record the every aspects of the simulation and write them into '.mat' files for analysis in MatLab.  
### Model   
A common used energy model is implemented as a standard model.  
### MatLab Interfaces   
The simulator can be called from MatLab and outputs results automatically after simulation ends.  
