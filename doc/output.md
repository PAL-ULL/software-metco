# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) -- Version registered at Safe Creative

## Output

Currently, METCO only allows two formats for the results files, plain text and XML. The differences between each format and how to interpret them are explained down below.

## Plain Text

Let's supposed that we are trying to find the global optimum of the [Ackley Function](https://www.sfu.ca/~ssurjano/ackley.html) (30 dimensions instance) using the Differential Evolution algorithm implemented in METCO. We should run our experiment like:

`./metcoSeq /home/username . PlainText results.txt MonoDE Ackley EVALUATIONS 100000 1000 0 0.5 0.115 100 ! 30 $ NoOp;`

After it finishes, we will have the **result.txt** file with the results of the experiment for every **1000** evaluations of the algorithm. If you don't realize why it is this way, check out [how to run METCO](setup.md).

Next, there is an example of the [result file](test.output)

```
Current Evaluation = 1000(1000)
Front Size = 1
4.14602569773142449 10.1111731614466649 -7.0645145754332308 5.28332344110465524 -16.4433158111848527 -19.38260894152085 -1.53641686320325022 3.04215541981260351 -32.7680000000000007 -14.8349038612781605 -28.78761358457227 16.8081287233420369 32.7680000000000007 -3.09718953467588065 -24.5689186668363035 -19.1237361386659259 20.0588857362693602 -5.64246610141288318 -32.6682047118675953 -6.03445241827631662 -32.7680000000000007 -32.7680000000000007 32.7680000000000007 -11.7832334497008624 -28.6204749126194393 -32.7680000000000007 -20.1861851748052032 6.89767668473053419 -0.531230880984679743 17.1323408588079502 21.7182817872359735 

Current Evaluation = 2000(2000)
Front Size = 1
-24.35348208226333 -32.7680000000000007 -8.50852747168975654 -7.35352755418119219 -32.7680000000000007 -24.6259077116483347 27.7638852836433259 27.6237015356904365 -1.60598143843095187 -7.13597008083759121 -24.6687523270085229 27.7180705161420953 -32.7680000000000007 7.72639757134690619 25.9290196501356647 32.7680000000000007 32.7680000000000007 6.15664724636666882 19.4532477660166414 -3.32440732729268262 -29.3155716412680079 32.7680000000000007 -12.3974223137411332 -6.06670901008821595 32.4274260862671895 15.1634840921440635 -13.7380409152107479 -32.0627234226757309 28.9572169925110501 -32.7680000000000007 21.7182817872359735

(...)

Genetic Algorithm DE
Number of Evaluations = 100000
Weight factor = 0.5
Crossover = 0.100000001490116119
Population Size = 100

Time =  0.468252999999999975

Current Evaluation = 100000
Front Size = 1
32.7680000000000007 -15.9025481981702868 32.7680000000000007 -16.0435141713351683 -11.9267453539161483 -8.90274492002194684 -19.0898707743117981 20.1607823281172145 9.15719495930455274 -28.770813669048227 10.4724050928404644 -30.2265264212207967 0 -19.3500226426485327 -16.6668501665244193 -18.9441218480358593 5.213448917308666 -1.21794220431338829 20.7714501836740055 10.2722986055862968 -19.4581973897388814 -11.5055220812062799 1.07924994624690029 24.6890939986446298 6.61085563166696843 -19.7870624185111659 1.59072514066614978 32.7680000000000007 -25.0643355725077868 -18.8879885874006135 21.7182817872359735 
```
**Notice that the results are not sorted in any particular way**.

The result files follows this format:

When the print period is reached, METCO shows the status of the execution:

* Current Evaluation
* Number of items of the Pareto's Front. In this case, as we are working with a single-objective function, the value is one.
* Evaluation of every single individual in the population.

This information is printed continuosly after the algorithm performs _printPeriod_ evaluations. 

In the end, when the execution is finished; besides the finals results, METCO shows the elapsed time in this execution and the algorithm information described in the [method printInfo](newPlugin.md) of the algorithm.

## XML
_Coming soon..._