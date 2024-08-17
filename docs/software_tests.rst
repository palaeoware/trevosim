.. _software_tests:
.. include:: <isonum.txt>

Tests
=====

TREvoSim has tests covering every function used in the simulation process. If you are a user, and would like to see the outputs of the tests, then this is possible by hitting the Test button on the toolbar menu, or Tests in the command menu. When this is selected, the main window is first split into two, and the software runs its test suite. The output of the results is then displayed on the panel on the right, shown below.

.. figure:: _static/windowLayout_test.png
    :alt: The TREvoSim graphical user interface in test mode
    :align: center

    TREvoSim in test mode

The output text in the test log describes each test, and also the expected output. The system then makes sure the output meets these expectations. Failed tests will be highlighted in green, and explain why they have failed. A small number of the tests are, by necessity, stochastic and will occasionally fail. In this case, a sensible first step is to repeat that test to see if this happens a second time. This functionality is provided so it is possible for the user to check TREvoSim is running as expected and described in the publications introducing the model: if there is an element of the TREvoSim functionality that is of particular importance to your science, the tests help provide insight into the mode of operation of that part of the simulation, and the coders' expectations for how it works. More broadly, if you are interested in how the model runs, beyond looking at the source code, you can alternatively/additionally output a working log to check the internal workings of the software - see :ref:`loggingsim`. 