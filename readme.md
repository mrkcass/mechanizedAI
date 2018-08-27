<span style="font-size:larger;">**mechanizedAI**</span>

Mechanized AI, as it's name suggests is the application of AI by mechanical means.
mechanizedAI is a framework of design and code on which useful AI can be built and
deployed.

&nbsp;

<span style="font-size:larger;">**Somax**</span>

**More concretely mechanized AI is Somax.**

Somax adds sensors and motion to an AI inference and
data collection engine. The sensors feed the AI with raw information. The AI
uses the information to infer whether it should collect data or provide assistance.
If data should be collected then sensors are directed by motion or other means
and data is then captured. If instead assistance is needed motion or other means
are used to provide the assistance directly or indirectly.

**Hardware**

Motion is provided by way of a 4 axis gimbal attached to a 3D color depth of field
camera that can capture color images with per pixel depth at 1280x720 resolution
at 30 frames a second. The gimbal has 800 steps per full revolution and completes a
90 degree directional change in the same time as a human. In addition, a 4 channel
MEMs microphone array captures 48 khrz of synchronzied sample per microphone. Somax
provides AI inference at a sustained maximum rate 400 GFLOPS Somax. Currently Somax
operates off a single 12 volt 1.5 amp power supply. Future version will shed the
on board ac supply in favor of a lower overall foot print using 4x 18650 cells and
should be able to run all systems at full power for about an hour.

**Software**

Deciding what to do is an AI task called inference. Before inference can begin
an AI must first be trained. Of the two, training is a much a bigger challenge than
inference.

Training involves repeatedly passing millions of multidimensional floating
point arrays as examples, for or against, though a compuation graph that usually
has on the order of millions of nodes and weighted edges. After each example of
data is passed through the graph, the weights of all edges going into all nodes
are then, increased or decreased by a very, very small amount. this takes a lot
computational power and lot of time. Training is usually the domain of GPU's which
seem purpose built for the task. They speed things up but in the process use a lot
of power. Most training these happens on NVidia 900 class or better hardware. At full
power the cards easily consume 200 watts of power. the average phone has less than
4000 milliamp hours of power at under 5 volts of output. In other words you phone
has enough power to run a a 900 class GPU, full out for about .2 seconds if it was
even able to release all it power that fast in ther first place. In extreme cases this
power draw must continue for days and even weeks.

Inference is another story and can easily be executed on a portable device in
a reasonable amount of time. Somax uses 4 Myraid 2 compute engines like the one's
found in the neural compute sticks. Each deleivers 100 GFLOPS of computation for
1 watt of power used. To give some scale, one Myriad 2 connected to a raspberry 3
increases it's computational power by 9x (https://movidius.github.io/blog/ncs-rpi3-mobilenets/).
So when the wind is blowing at our back, somax should be able to process 36x
a more inferences in the same amount of time when compared to a raspberry pi 3.

Mechanized AI will offer an end to end solution to address training and inference.
The AI mech will infer, capture, and interact. When ready to train, the mech will
attach to a DreamBox and begin training. the AI mech is always in charge of the
training. The dreambox has no method to alter the mach AI and serves as the muscle
for training. When training is complete, the mech will upload the new models, clean up,
wake up and begin again with capture, infer, and interact functions. I hope to make
this like the apple of AI devices. It does only a few things but it does them very
well, without much help and everytime :-)

&nbsp;


<span style="font-size:larger;">**Licensing**</span>

Somax is my attempt to offer personal, private, and soley owned AI to
everyone.

To that end there is no license for any design or implementation embodied within
the domain mechanizedAI.

But there is a request.

Use it to make the world better for everyone :-)

**-mechanizedAI**