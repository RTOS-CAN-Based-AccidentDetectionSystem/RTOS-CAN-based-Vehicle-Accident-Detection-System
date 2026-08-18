# GSM Module Working

## Overview
This module uses SIM800L GSM module to send SMS alerts.

## Working
1. Initialize GSM using AT commands
2. Set SMS mode to text
3. Send SMS using AT+CMGS command

## Flow
STM32 → UART → SIM800L → Mobile SMS

## Baud Rate
9600

## Output
SMS received on mobile when triggered
