# gait-analysis
A user friendly low cost wearable gait analysis prototype for basic gait monitoring using FSR sensors and accelerometers.

##Overview
We ahve designed a  flexible smart insole prototype integrating multi pleplantar pressure sensors and an ADXL accelerometer to measure pressure distribution and foot motion during gait cycles. 

## Hardware
| Component | Role |
|---|---|
| 4× FSR-402 sensors | Plantar pressure at anatomical zones |
| ADXL-345 accelerometer | Foot kinematics via I2C |
| Arduino Uno | Data acquisition & processing |
| MicroSD module | Portable data logging via SPI |

<img width="638" height="743" alt="image" src="https://github.com/user-attachments/assets/db36be43-56b3-4321-9afb-7888bc944406" />

## Sensor Placement
Sensors placed at anatomically significant plantar zones:
i. Sensor 1 (S1) : First Meta tarsal Head.
ii. Sensor 2 (S2) : Near second Meta tarsal Head.
iii. Sensor 3 (S3) : Base of 5th Metatarsal (Lateral Arch).
iv. Sensor 4 (S4) : Calcaneal Tuberosity(Heel)

<img width="834" height="511" alt="image" src="https://github.com/user-attachments/assets/7ca63d9d-eb92-4408-8c90-2f80e99b69af" />

## Signal Processing
**Hardware filtering:** 1nF ceramic capacitors as low-pass 
filters per FSR; 470μF capacitor for SD module voltage stability.

**Software filtering:** Digital Exponential Moving Average (EMA):

Y(n) = α·X(n) + (1 - α)·Y(n-1)    [α = 0.5]

**Force conversion:** ADC readings converted to Newtons via 
voltage divider model and FSR conductance curve.

## Results
- Identified all major gait phases: heel strike, foot flat, 
  heel off, toe off, swing
  <img width="818" height="370" alt="image" src="https://github.com/user-attachments/assets/58eaf602-a50e-4a78-a809-87ec528f3c1a" />
  [A: Before stepping, B: Heel strike, C: Flat position, D: Start of heel off, E: Full heel off, F: Toe off]

- Pressure heatmap showing anatomically 
  accurate load distribution
  <img width="343" height="527" alt="image" src="https://github.com/user-attachments/assets/9f6ae269-13b2-4759-81e7-a219b1d7c832" />

  <img width="451" height="542" alt="image" src="https://github.com/user-attachments/assets/66e38424-001a-4eb1-9f2c-5c0bbdefe3f9" />

  
- Acceleration curves matching expected kinematic patterns 
  per gait phase
  <img width="819" height="450" alt="image" src="https://github.com/user-attachments/assets/54a79071-024d-4542-91f4-9119fe9cc815" />
  [A: Before stepping, B: Heel strike, C: Flat position, D: Start of heel off, E: Full heel off, F: Toe off]

## Team
Nabila Tanzim Tahwa · Tahera Tabassum Afreen · Ilma Binta Alim    
Department of Robotics and Mechatronics Engineering
University of Dhaka

