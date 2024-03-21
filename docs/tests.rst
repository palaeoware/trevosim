.. _tests:

Tests
=====

TREvoSim has tests covering every function used in the simulation process. If you are working on the TREvoSim code, be aware that the tests are run as part of the build chain -- when a test fails, so will the build. However, if you are a user, and would like to see the outputs of the tests, then this is possible by hitting the Test button on the toolbar menu, or Tests in the command menu. When this is selected, the main window is first split into two, and the software runs its test suite. The output of the results is then displayed on the panel on the right, shown below.

.. figure:: _static/windowLayout_test.png

As such, it is possible to check TREvoSim is running as expected and described in the publications introducing the model (you can alternatively/additionally output a working log to check the internal workings of the software - see :ref:`loggingsim`). 

The output in the test log describes each test, and also the expected output. The system then makes sure the output meets these expectations. Failed tests will be highlighted in green.