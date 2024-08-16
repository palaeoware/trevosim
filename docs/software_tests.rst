.. _software_tests:
.. include:: <isonum.txt>

Tests
=====

TREvoSim has tests covering every function used in the simulation process. If you are a user, and would like to see the outputs of the tests, then this is possible by hitting the Test button on the toolbar menu, or Tests in the command menu. When this is selected, the main window is first split into two, and the software runs its test suite. The output of the results is then displayed on the panel on the right, shown below.

.. figure:: _static/windowLayout_test.png

If you are working on the TREvoSim code, and wish to check all tests are passing, you can do this by: running them in the software as outlined above; by running the binary TREvoSimTest -- created during a build -- from the console (double clicking is also possible on Windows); or using the option within QtCreator (Tools |rarr| Tests |rarr| Run all tests...; note is may be necessary to increase the test timeout settings in QtCreator's options if using this approach). The output in the test log describes each test, and also the expected output. The system then makes sure the output meets these expectations. Failed tests will be highlighted in green.

These are provided so it is possible for the user to check TREvoSim is running as expected and described in the publications introducing the model: if there is an element of the TREvoSim functionality that is of particular importance to your science, the tests help provide insight into the mode of operation of that part of the simulation, and the coders' expectations for how it works. More broadly, if you are interested in how the model runs, beyond looking at the source code, you can alternatively/additionally output a working log to check the internal workings of the software - see :ref:`loggingsim`. 