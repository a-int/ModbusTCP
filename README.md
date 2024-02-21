# Table of contents

1. [Overview](#overview)
2. [Demonstration](#demo)

## Overview <a name="overview"></a>

### What

The focus of this project was support of Modbus TCP over ethernet on stm32F407.

### Why

Modbus TCP/IP is a variant of the Modbus family of communication protocols designed for the supervision and control of automation equipment over Ethernet networks. It is an application layer messaging protocol that works in conjunction with Ethernet to enable client/server communication between devices connected on different types of networks. The primary purpose of Modbus TCP/IP is to provide a standardized TCP interface that allows Modbus devices to communicate seamlessly over an Ethernet network, enabling efficient and reliable data exchange. It is widely used in industrial automation, building automation, energy management, remote monitoring, and transportation due to its reliability, ease of integration, and compatibility with various devices. The protocol's focus on simplicity and robustness has led to its widespread adoption in various industrial applications.

### How 
All parsing and error checking is done on site. The user just need to add functions with concrete actions to read or write requested data from/to source used to store data. The following functions are:
```
	//--------for reading commands-----------------//
	void read_coils(char* repl_buf, uint16_t address, uint16_t quantity);
	void read_discrete(char* repl_buf, uint16_t address, uint16_t quantity);
	void read_holdings(char* repl_buf, uint16_t address, uint16_t quantity);
	void read_inputs(char* repl_buf, uint16_t address, uint16_t quantity);
	//--------for writing commands-----------------//
	void write_single_coil(uint16_t address, uint16_t val);
	void write_single_holding(uint16_t address, uint16_t val);
```

In this work it was necessary to write software code for a STM32F4 microcontroller to communicate over Modbus protocol with external device (PC) over Ethernet.
Supported commands are: 
- 0x01 - read a coil;
- 0x02 - read discrete inputs;
- 0x03 - read holding registers;
- 0x04 - read input registers;
- 0x05 - write a single coil;
- 0x06 - write a single holding registers.
Supported error handlers are: 
- 0x01 - unsupported commands;
- 0x02 - wrong data address or quantity is requested;
- 0x03 - wrong value to write is provided;
