# Reusable lab notebook

Copy this document per experiment; replace the prompts with your own evidence.

| Field | Your observation |
|---|---|
| Chapter / program | |
| Date, OS, compiler | |
| Hypothesis | |
| Server command | |
| Client command | |
| Expected result | |
| Actual result | |
| Evidence (terminal output / packet numbers) | |
| Explanation using API return values | |
| Code change and rationale | |
| Regression checks | |

## Before running

Identify the listener, connected descriptors, buffer capacities and framing rule.
Predict which call can block. Predict what `recv()` returning 0 would mean for
this socket type and requested length.

## After running

Explain an observation that surprised you. Change only one variable (payload
size, address, concurrency model or peer behavior), rerun and compare. Report
measurements with the setup and limitations; never infer a production benchmark
from a single loopback result.
