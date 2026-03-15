# ACI AES S-Box — ISW Masking Implementation

This repository contains the implementation-focused parts of a coursework project from the **Advanced Cryptographic Implementations** course at the **Technical University of Munich (TUM)**.

The project implements a **masked AES S-box using the ISW masking scheme** to protect against side-channel attacks. The implementation was developed in C inside a course-provided embedded development environment based on **OpenTitan, the Ibex RISC-V core, and QEMU simulation**.

The repository preserves the **student-implemented components** of the project as a portfolio artifact.

---

## Project Overview

Modern cryptographic implementations must protect secret data not only against mathematical attacks but also against **physical leakage**, such as power consumption or electromagnetic radiation.

One of the most widely used countermeasures against such attacks is **masking**, where intermediate values are split into multiple random shares so that no single value reveals the secret.

This project implements a **higher-order masked AES S-box using the ISW scheme (Ishai–Sahai–Wagner)**. The implementation demonstrates:

* masked finite-field arithmetic in **GF(2⁸)**
* secure multiplications between masked values
* masked inversion in the AES field
* reconstruction of the AES S-box
* validation of the implementation against the standard AES S-box

---

## Repository Contents

This repository contains only the **implementation-relevant files** developed or modified as part of the coursework:

```
src/
 ├─ aes_masking_isw.c   # Main masked AES S-box implementation
 ├─ aes_masking_isw.h   # Function interfaces and masking configuration
 └─ aes_const.h         # Reference AES S-box and GF arithmetic tables
```

These files were originally integrated into a much larger embedded development environment.

The surrounding framework (OpenTitan codebase, build system, and simulation environment) is **not included in this repository**.

---

## Implementation Details

The implementation demonstrates several core concepts used in side-channel resistant cryptographic software.

### Masked Representation

A secret value `x` is split into multiple **shares**:

```
x = x₀ ⊕ x₁ ⊕ ... ⊕ x_d
```

where `d` is the masking order. In this implementation:

```
MASKING_ORDER = 3
```

as defined in the header file. 

---

### Secure Multiplication

Secure multiplication between masked values is implemented using the **ISW multiplication protocol**, which introduces fresh randomness to prevent leakage during intermediate computations.

The implementation uses random values generated via a software PRNG to refresh masks and secure intermediate values.

---

### Masked AES S-Box

The AES S-box is computed via the standard decomposition:

1. Compute multiplicative inverse in **GF(2⁸)**
2. Apply the AES affine transformation

The implementation performs these operations on masked shares and reconstructs the result afterwards.

Correctness is validated against the reference AES S-box table defined in `aes_const.h`. 

---

### Finite Field Arithmetic

The implementation includes branch-free arithmetic for operations in **GF(2⁸)**, including:

* field multiplication
* exponentiation
* lookup tables for log/exp representation

These operations are required for implementing the masked inversion used in the AES S-box.

---

## Environment Context

The code was originally developed inside a **course-provided embedded development environment** that included:

* OpenTitan firmware framework
* Ibex RISC-V processor model
* QEMU-based simulation
* UART logging infrastructure
* a VirtualBox development image

The main program integrates with the OpenTitan runtime and uses UART logging to output the computed S-box values and validation results. 

Because the full environment is large and provided by the course infrastructure, it is **not included in this repository**.

---

## Project Status

This repository is preserved as a **portfolio artifact** demonstrating:

* implementation of side-channel resistant cryptographic algorithms
* low-level C development
* work inside a large embedded firmware codebase

It is **not intended to be a standalone buildable project**.

---

## Course Context

The project was developed as part of the TUM course:

**Advanced Cryptographic Implementations**

Topics covered in the course included:

* side-channel attacks and countermeasures
* higher-order masking schemes
* masked implementations of cryptographic primitives
* embedded cryptography
* efficient finite-field arithmetic

---

## License

This repository contains coursework implementation artifacts and is provided for **demonstration and portfolio purposes**.
