# Development Environment Notes

This project was originally developed inside a larger embedded development environment used for experimenting with cryptographic algorithms and their secure implementations.

The goal of the assignment was to implement and analyze **masked cryptographic primitives** within a realistic firmware stack rather than as isolated standalone programs.

---

# Embedded Development Setup

The development environment consisted of a preconfigured virtual machine containing a full embedded firmware toolchain.

Key components included:

* a RISC-V based processor model
* firmware libraries for device interaction
* simulation tooling for executing firmware binaries
* UART-based logging for debugging and output
* build scripts and firmware tooling

This environment allowed the implementation to run as part of a simulated firmware application.

---

# Firmware Integration

The AES masking implementation was integrated into a firmware example program.

The program entry point performs the following steps:

1. initialize device peripherals
2. configure UART logging
3. execute validation routines
4. compute the masked AES S-box
5. verify results against the reference AES S-box

The output is printed through the UART logging interface.

---

# Scope of This Repository

The original development environment contains a large firmware codebase, build infrastructure, and generated artifacts.

To keep this repository focused and lightweight, it includes **only the files that were directly implemented or modified** for the cryptographic algorithm.

These files contain:

* masked arithmetic operations
* secure multiplication routines
* masked inversion
* AES S-box construction
* reference tables for field arithmetic

All external framework code and build infrastructure have intentionally been omitted.

---

# Purpose of This Repository

The repository is preserved as a **demonstration of implementation techniques used in side-channel resistant cryptography**, rather than as a complete firmware project.

Its purpose is to document the cryptographic implementation and highlight the core algorithms developed during the project.
