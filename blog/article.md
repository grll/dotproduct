---
title: A Deep Dive into the Optimization of Dot Product Implementations
author: Guillaume Raille
date: 24.03.2024
---

# Introduction

## Synopsis

In this technical blog post, we delve into the intricate world of performance
optimization for dot products - a key operation in machine learning. We'll dissect why
and how it plays a pivotal role in current machine learning practices. The journey
encompasses an assortment of implementation methodologies; from a basic Python
implementation to a Numpy-based approach leveraging vectorization, to exploring raw
performance in C, and speed advantages of ASM OpenBLAS. We will not stop there and will
further explore the realms of parallelization with openMP and extend our reach to
GPU-based computation with CUDA.

---
**NOTE**

Take into consideration the following if you want to follow along:

* The code for all the experiments presented in this article and more is available on
the [repository](https://github.com/ ..)
* The CUDA part will require a CUDA enabled GPU to run see list of CUDA enabled GPU.
* The ASM OpenBLAS part is using instructions for the AMD Zen Architecture (x86_64)
similar to the Intel HASWELL architecture. It has been tested (successfully) only on a 
AMD Threadripper 1950X.

---

## The Importance of Dot Product

Viewed merely as a mundane operation from linear algebra, the dot product might seem
inconsequential. However, it's an operation that sits at the heart of today's machine
learning and deep learning architectures, making its optimization vital.

To refresh your memory, the dot product operation multiplies corresponding dimensions of
two vectors and sums the results. In formal mathematical terms, it's represented as:

{Write latex here showing the dot product between two vectors a and b of arbitrary dimensions}

Machine and deep learning paradigms are deeply entrenched in linear algebra. One of the
prominent examples that underscores the indispensability of the dot product is the
activation function of a neuron in a neural network:

{Insert latex of the activation function (sum of w * x)}

This operation forms the backbone of the ubiquitous 'fully connected' layer - a series
of such activation functions running parallelly within a layer.

Evidently, the dot product is a recurring element in deep learning. Yet, when performed
naively in a 'for' loop, it becomes highly inefficient. Given the massive scale of
today's LLM models - like GROK-1 with a staggering 314 billion weights - efficient
execution of dot products is not just a nice-to-have, but a necessity. This necessity
paves the path for constant optimization research, allowing us to scale heights in
computing performance. 

# Practical Experimental Setup

## Experimental Setup

The crux of our exploration pivots around practical experiments. In order to conduct
these, I first needed to create an apt dataset. Taking inspiration from actual neural
network operations, where the activation of a neuron often oscillates between 0 and 1, I
have created the dataset similarly. The dataset consists of two vectors (w and x), each
populated with one million float32 values randomly falling between 0 and 1.

This ensures the problem size is sufficiently large, enabling us to observe discernible
differences between the efficiency of various approaches tested.

The experimental sandbox for our performance showdown is my personal system, equipped as
follows:

* CPU: AMD Threadripper 1950X
* GPU: Nvidia RTX 2070
* OS: Ubuntu 22.04 LTS

For our C-based implementations, the code has been compiled using gcc.

This setup should ensure precise, controlled and consistent test conditions, providing a
reliable platform for our investigation into dot product performance optimization.

## Experimental Analysis: Python Implementations of Dot Product

* The Cost of Naivety: Benchmarking a Loop-based Implementation in Python
* The Speed of Sophistication: Benchmarking a Numpy-based Implementation
* Going Deeper: Why Vector Operations are Faster

## Exploring C Implementations of Dot Product

* Speed without Python Overhead: Implementing Dot Product with Loops in C
* Comparing Different Levels of Compilation Optimization: -O1, -O2, -O3

## Pushing Speed Boundaries with Assembly

* Experimenting OpenBLAS Assembly Code in C
* Why Numpy Still Leads: The Power of Parallelization

## Parallelization: An Intro to OpenMP
    Upgrading Assembly Code with Parallelization
        The Mighty Combination: OpenBLAS Assembly Code and OpenMP
    Leveling Up with CUDA
        Unleashing the CUDA Power: Experimental Performance Demonstration
    Conclusion: Diving Deeper into Dot Product Implementation for Optimization
