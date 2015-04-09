# Details #

# Introduction #
As discussed previously, my proposal is to write a web server which distributes repetitive http
queries across a farm of n query execution servers, and then combines and returns the
aggregate results.
# Use Case 1 – standard workflow #
## Precondition ##
An actor has a list of 10 queries to execute against a remote server out of the actor’s control,
e.g:
http://www.xyz.com/do_something?arg1=1
http://www.xyz.com/do_something?arg1=2
http://www.xyz.com/do_something?arg1=3
http://www.xyz.com/do_something?arg1=4
http://www.xyz.com/do_something?arg1=5
http://www.xyz.com/do_something?arg1=6
http://www.xyz.com/do_something?arg1=7
http://www.xyz.com/do_something?arg1=8
http://www.xyz.com/do_something?arg1=9
http://www.xyz.com/do_something?arg1=10
## Postcondition ##
The following (in order) response is returned to the actor:
xyzresult1
xyzresult2
xyzresult3
xyzresult4
xyzresult5
xyzresult6
xyzresult7
xyzresult8
xyzresult9
xyzresult10

### Description ###
The full request Qx where x = number of querystrings, is built by the actor as one long string
delimited by linefeeds, and this is passed by http to the Control Server. The Control Server then
breaks the querystrings into x/n sets S1, S2, …Sn, where n = number of Query Servers, and sends
each set to a unique Query Server via http. Each Query Server in turn initiates an http request
in a new thread for each querystring in the set it received. Each thread has a timeout T after
which it returns either a null or a result to the Query Server’s main logic, where results are
combined into one result set Rn and returned to the Control Server. As each Query Server
returns it’s result set, the Control Server combines this data into one large result set and
returns it to the actor.
# Use Case 2 – exceptional workflow #
If Query Server threads fail to return data within time T, the Query Server’s main logic ignores
it’s results. If a Query Server fails to return a dataset to the Control Server within time T', the
Control Server ignores it’s results.

![http://www.eanh.net/images/deploymentdiagram.jpg](http://www.eanh.net/images/deploymentdiagram.jpg)

# Backup Proposal #
In the event that this project becomes unworkable, I plan to remove the distributed element of the project, and simply have the control server manage the multiple threads which query the remote web server.


# Additional Requirements Per Todd Nugent #

  * add mult clients talking to server
  * add timings on each server
  * and overall timings
  * add headers