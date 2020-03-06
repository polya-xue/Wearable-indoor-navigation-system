Wearable indoor navigation system for blind people based on "iBeacon" :
The navigation system consists of a intelligent walking stick for blind people and an APP in smart phone.There are three modules:
1) Ultrasonic obstacle avoidance module:
The ultrasonic obstacle avoidance module is composed of three ultrasonic sensors interacting with each other at 120°.They provides real-time feedback on the obstacles around the user by APP.
2) Motion monitoring module:
It uses six-axis sensor module MPU6050 to identify  gesture of users.For example, when the user knock down the ground with the blind stick for three times, it will trigger the indoor positioning function, and the App will broadcast the current position information immediately.Another case:when the user has a fall, the alarm function will be triggered automatically. If the user fails to cancel the alarm after a certain time, the system will automatically call the family member's phone to notify them.
3) Voice broadcast on App:
Achieved in the Android platform to support the above functions.

Implementation details:
1) ultrasonic obstacle avoidance: 
Setting the safety threshold, and activate the alarm when the distance captured by ultrasonic function is bigger than the threshold;
2) Moving monitoring: 
This model can distinguish user's state of motion by seting corresponding shreshold to different accelerated speed,including the acceleration of normal action, the acceleration of waving arm and the acceleration of a sudden fall.
3) Indoor positioning: 
Many beacons with corresponding position information are set up in indoor areas(such as shop) in advance.They will broadcast the iBeacon package constantly. The receiver (the user) can identify their position by receiving broadcasting package.Specifically,the App plays a role as iBeacon reciever which will recieve broadcast packages from 3 beacon with the strongest signal.The reciever  calculates the distance information according to the rssi signal, and then use Trilateration algorithm, finally the address position of the user can be obtained.

Image show
![Image text](https://github.com/polya-xue/Wearable-indoor-navigation-system/blob/master/picture/show.png)

