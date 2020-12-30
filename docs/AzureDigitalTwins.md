# Adding Azure Digital Twins nodes to the sample solution

Azure Digital Twin instance is already installed as a part of the sample solution.  However, in order to use Digital Twins, you must register Digital Twin models written in Digital Twin Definition Language (DTDL), then create digital twin nodes using these models.

## Digital Twin Model

User defined twin models are used to build your twin graph in the self-defined terms of your needs.  A model defines characteristics of physical objects, such as Room or Sensor.  A model contains data such as properties, telemetry, events, and commands.

You can create digital twin models with API or various tools.  In this instruction, we will use ADT CLient sample application to create digital twin models.

## Digital Twin

A digital twin is an instance of your digital twin models.  It can connect to other digital twin(s) via relationships to for a twin graph.  The twin graph is the representation of your entire environment.

You can create digital twins with API or various tools such as AZ CLI with Azure Digital Twins extension.  In this instruction, we will use ADT Client sample application to create digital twins.

## Preparation

1. [Set up ADT Client App](ADTClient.md) to create digital twin model and digital twins
1. [Set up ADT Explorer](ADTExplorer.md) to visualize digital twin graph

## Creating digital twin models and digital twins

In this hands on lab, we will create a digital twin graph for a building

Create digital twin models and digital twins with ADT Client app with :

```bash
SetupBuildingScenario
```

This command will create :

- Digital Twin models for Floor, Room, and Thermostat
- Digital Twins for `Floor 1`, `Room 141`, and `Thermostat 141`
- Relationships between `Floor 1`, `Room 141`, and `Thermostat 141`

We will set `temperature` property of `Thermostat 141`.  Since temperature data from `Thermostat 141` represents temperature of `Room 141`, we will then set `temperature` property of `Room 141`.

## Visualizing Digital Twin Graph

In ADT Explorer, run query ``