# Tabos.VM
<b>Tabos.VM</b> is a virtual machine that aims at making easy for developers of hobby kernels to execute managed code on their platforms.

## Documentation
The source code uses the <b>oxygen</b> syntax to document functions and objects used by the VM, this will help developers on their journey exploring the VM's capabilities.

A more in-depth documentation will be available soon. It will contain all necessary knowledge to implement the VM in personal projects and simple code examples to run as tests.

## Principles
The <b>Tabos.VM</b> project is based off a few principles to be followed along its development.
  - <b>Simple implementation</b> <br/>
    The virtual machine should be easy enough to be implemented by beginners;
  - <b>Powerful functionalities</b> <br/>
    Tabos.VM's functionalities must be complete enough such that programs compiled to its bytecode should be able to perform complex instructions;
  - <b>Low level compatibility</b> <br/>
    Even though Tabos.VM's bytecode will be the product of high-level source codes, it should still be able to perform low level actions, such as handling interrupts, gather cpu information and interact with co-processors, such as the FPU. <br/>
    This will fulfill requirements from more expert users that wish to extend their kernels with extensions;
  - <b>High level typechecking</b> <br/>
    Tabos.VM's typecheck system should be able to handle modern types, such as union types and value types.

## Projects using Tabos.VM
- <a href="https://github.com/TheOSDevOrg/strap-v2">strap-v2</a>: a hobby kernel from Nik300 that aims at being a microkernel extended by modules interpreted from the Tabos Virtual Machine.