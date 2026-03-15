# Masked AES S-Box Implementation (ISW Scheme)

This repository contains the implementation-focused parts of a project exploring **side-channel resistant cryptographic implementations**.

The code demonstrates how the AES S-box can be implemented using **higher-order masking techniques**, specifically the **ISW masking scheme (Ishai–Sahai–Wagner)**, in order to protect intermediate values against side-channel leakage.

The implementation was developed in C within an embedded development environment based on a **RISC-V firmware stack and hardware simulation tools**.

This repository preserves the **core implementation files** as a portfolio artifact.

---

# Project Overview

Cryptographic algorithms deployed on real hardware can leak secret information through physical side channels such as:

* power consumption
* electromagnetic emissions
* timing behavior

To mitigate these attacks, sensitive values can be split into multiple **randomized shares**.
Masking ensures that no individual intermediate value directly reveals the underlying secret.

This project demonstrates the implementation of a **masked AES S-box** using the **ISW masking scheme**, including:

* masked finite-field arithmetic in **GF(2⁸)**
* secure multiplications on shared values
* masked inversion in the AES field
* reconstruction of the AES S-box output
* validation against the reference AES S-box

---

# Repository Structure

The repository intentionally contains **only the implementation-relevant components**.

```text
src/
 ├─ aes_masking_isw.c
 ├─ aes_masking_isw.h
 └─ aes_const.h
```

These files implement:

* masked arithmetic primitives
* secure multiplication
* masked inversion
* the masked AES S-box computation
* reference tables for AES field arithmetic

The surrounding firmware framework and simulation environment used during development are **not included**.

---

# Implementation Details

## Masked Representation

A value `x` is split into multiple shares:

```
x = x₀ ⊕ x₁ ⊕ ... ⊕ x_d
```

where `d` denotes the **masking order**.

Each share individually appears random, while their XOR reconstructs the original value.

---

## Secure Multiplication

Multiplication between masked values is implemented using the **ISW multiplication protocol**, which introduces fresh randomness to prevent leakage during intermediate computations.

Randomness is generated via a software PRNG and used to refresh mask values during secure operations.

---

## Masked AES S-Box

The AES S-box computation follows the classical decomposition:

1. multiplicative inverse in **GF(2⁸)**
2. affine transformation

Both operations are executed on **shared values**, ensuring that intermediate states remain masked throughout the computation.

---

## Finite Field Arithmetic

The implementation uses branch-free arithmetic for operations in **GF(2⁸)**, including:

* multiplication using log/exp tables
* exponentiation
* affine transformations

These primitives are required for constructing the masked inversion used in the AES S-box.

---

# Development Context

The implementation was originally developed within a **larger embedded firmware environment** used for experimentation with cryptographic algorithms.

The environment included:

* a RISC-V processor model
* firmware libraries and runtime support
* UART logging utilities
* a simulation setup for executing firmware images

Within this environment, the program computes the masked AES S-box and verifies correctness against a reference table.

Because this environment is large and contains external dependencies, it is **not included in this repository**.

---

# Project Status

This repository is preserved as a **technical portfolio artifact** demonstrating:

* implementation of side-channel resistant cryptographic primitives
* low-level C development
* work within a large embedded firmware codebase

The repository is **not intended to be a standalone buildable project**.
