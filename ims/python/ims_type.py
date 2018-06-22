###############################################################################
# Copyright (c) 2018 Airbus Operations S.A.S                                  #
#                                                                             #
# This program and the accompanying materials are made available under the    #
# terms of the Eclipse Public License v. 2.0 which is available at            #
# http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License   #
# v. 1.0 which is available at                                                #
# http://www.eclipse.org/org/documents/edl-v10.php.                           #
#                                                                             #
# SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause                            #
###############################################################################
 
#!/usr/bin/env python3

import xml.etree.ElementTree as ET

##############################################
#                   AFDX                     #
##############################################

class AFDX(object):
    def __init__(self):
        self.producedData = AFDXTypedData()
        self.consumedData = AFDXTypedData()

    def isEmpty(self):
        return not self.producedData.samplings and not self.producedData.queuings and not self.consumedData.samplings and not self.consumedData.queuings

    def convertToXml(self):
        afdx_element = ET.Element('AFDX')

        if not self.producedData.isEmpty():
            afdx_prod_element = ET.SubElement(afdx_element, 'ProducedData')
            for afdx in self.producedData.samplings.values():
                afdx_prod_element.append(afdx.convertToXml())
            for afdx in self.producedData.queuings.values():
                afdx_prod_element.append(afdx.convertToXml())

        if not self.consumedData.isEmpty():
            afdx_cons_element = ET.SubElement(afdx_element, 'ConsumedData')
            for afdx in self.consumedData.samplings.values():
                afdx_cons_element.append(afdx.convertToXml())
            for afdx in self.consumedData.queuings.values():
                afdx_cons_element.append(afdx.convertToXml())

        return afdx_element

    @staticmethod
    def parse(element):
        afdx = AFDX()

        if element is None:
            return None

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = element.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curTypeNode = afdx.consumedData
            else:
                curTypeNode = afdx.producedData

            for sampling_el in data_direction_el.findall('SamplingMessage'):
                curTypeNode.samplings[sampling_el.attrib['Name']] = AFDXSampling.parse(sampling_el)

            for queuing_el in data_direction_el.findall('QueuingMessage'):
                curTypeNode.queuings[queuing_el.attrib['Name']] = AFDXQueuing.parse(queuing_el)

        return afdx


    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        if elementToMerge is None:
            return 0,0

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = elementToMerge.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curDirNode = destCompoment.consumedData
            else:
                curDirNode = destCompoment.producedData

            for node_type_name in ['SamplingMessage', 'QueuingMessage']:
                for element in data_direction_el.findall(node_type_name):

                    if node_type_name == 'SamplingMessage':
                        curTypeNode = curDirNode.samplings
                        cur_data = AFDXSampling.parse(element)
                    else:
                        curTypeNode = curDirNode.queuings
                        cur_data = AFDXQueuing.parse(element)

                    if cur_data.name in curTypeNode:
                        other = curTypeNode[cur_data.name]
                        if not cur_data.isCompatible(other):
                            error = error + 1
                            print('Error : two incompatible definitions of AFDX %s :\n%s\n%s' % (cur_data.name, cur_data, other))
                    else:
                        curTypeNode[cur_data.name] = cur_data

        return error, warning

class AFDXTypedData(object):
    def __init__(self):
        self.samplings = {}
        self.queuings = {}

    def isEmpty(self):
        return not self.samplings and not self.queuings

class AFDXSampling(object):
    def __init__(self, name, localName, messageSizeBytes, validityDurationUs, periodUs=None):
        self.name = name
        self.localName = localName
        self.messageSizeBytes = messageSizeBytes
        self.validityDurationUs = validityDurationUs
        self.periodUs = periodUs

    def convertToXml(self):
        el = ET.Element('SamplingMessage',
                      Name=self.name,
                      LocalName=self.localName,
                      MessageSizeBytes=self.messageSizeBytes,
                      ValidityDurationUs=self.validityDurationUs)
        if self.periodUs is not None:
            el.set('PeriodUs', self.periodUs)
        return el

    @staticmethod
    def parse(element):
        period = None
        if 'PeriodUs' in element.attrib:
            period = element.attrib['PeriodUs']

        return AFDXSampling(element.attrib['Name'],
                     element.attrib['LocalName'],
                     element.attrib['MessageSizeBytes'],
                     element.attrib['ValidityDurationUs'],
                     period)

    def isCompatible(self, other):
        return self.localName == other.localName and self.messageSizeBytes == other.messageSizeBytes and self.validityDurationUs == other.validityDurationUs and self.periodUs == other.periodUs

    def __str__(self):
        return str(self.__dict__)

class AFDXQueuing(object):
    def __init__(self, name, localName, maxSizeBytes, queueDepth):
        self.name = name
        self.localName = localName
        self.maxSizeBytes = maxSizeBytes
        self.queueDepth = queueDepth

    def convertToXml(self):
        return ET.Element('QueuingMessage',
                          Name=self.name,
                          LocalName=self.localName,
                          MaxSizeBytes=self.maxSizeBytes,
                          QueueDepth=self.queueDepth)

    @staticmethod
    def parse(element):
        return AFDXQueuing(element.attrib['Name'],
                           element.attrib['LocalName'],
                           element.attrib['MaxSizeBytes'],
                           element.attrib['QueueDepth'])

    def isCompatible(self, other):
        return self.localName == other.localName and self.maxSizeBytes == other.maxSizeBytes and self.queueDepth == other.queueDepth

    def __str__(self):
        return str(self.__dict__)



##############################################
#                   Af429                    #
##############################################

class A429(object):
    def __init__(self):
        self.producedData = {}
        self.consumedData = {}

    def isEmpty(self):
        return not self.producedData and not self.consumedData

    def convertToXml(self):
        a429_element = ET.Element('A429')

        if self.producedData:
            a429_prod_element = ET.SubElement(a429_element, 'ProducedData')
            for bus in self.producedData.values():
                a429_prod_element.append(bus.convertToXml())

        if self.consumedData:
            a429_cons_element = ET.SubElement(a429_element, 'ConsumedData')
            for bus in self.consumedData.values():
                a429_cons_element.append(bus.convertToXml())

        return a429_element

    @staticmethod
    def parse(element):
        a429 = A429()

        if element is None:
            return None

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = element.find(node_name)
            if data_direction_el is None:
                continue

            for bus_el in data_direction_el.findall('Bus'):
                if node_name == 'ConsumedData':
                    a429.consumedData[bus_el.attrib['Name']] = A429Bus.parse(bus_el)
                    curBusNode = a429.consumedData[bus_el.attrib['Name']]
                else:
                    a429.producedData[bus_el.attrib['Name']] = A429Bus.parse(bus_el)
                    curBusNode = a429.producedData[bus_el.attrib['Name']]

                for sampling_el in bus_el.findall('SamplingLabel'):
                    curBusNode.samplings[sampling_el.attrib['LocalName']] = A429SamplingLabel.parse(sampling_el)

                for queuing_el in bus_el.findall('QueuingLabel'):
                    curBusNode.queuings[queuing_el.attrib['LocalName']] = A429QueuingLabel.parse(queuing_el)

        return a429


    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        if elementToMerge is None:
            return 0,0

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = elementToMerge.find(node_name)
            if data_direction_el is None:
                continue

            for bus_el in data_direction_el.findall('Bus'):
                if node_name == 'ConsumedData':
                    curBus = A429Bus.parse(bus_el)
                    curDirNode = destCompoment.consumedData
                else:
                    curBus = A429Bus.parse(bus_el)
                    curDirNode = destCompoment.producedData

                if curBus.name in curDirNode:
                    other = curDirNode[curBus.name]
                    err,warn = A429Bus.merge(other, bus_el)
                    error += err
                    warning += warn
                else:
                    curDirNode[curBus.name] = curBus

        return error, warning


class A429Bus(object):
    def __init__(self, name):
        self.name = name
        self.samplings = {}
        self.queuings = {}

    def convertToXml(self):
        bus_element = ET.Element('Bus', Name=self.name)

        for a429 in self.samplings.values():
            bus_element.append(a429.convertToXml())
        for a429 in self.queuings.values():
            bus_element.append(a429.convertToXml())

        return bus_element

    @staticmethod
    def parse(element):
        return A429Bus(element.attrib['Name'])

    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        for node_type_name in ['SamplingLabel', 'QueuingLabel']:
            for element in elementToMerge.findall(node_type_name):

                if node_type_name == 'SamplingLabel':
                    curTypeNode = destCompoment.samplings
                    cur_data = A429SamplingLabel.parse(element)
                else:
                    curTypeNode = destCompoment.queuings
                    cur_data = A429QueuingLabel.parse(element)

                if cur_data.localName in curTypeNode:
                    other = curTypeNode[cur_data.localName]
                    if not cur_data.isCompatible(other):
                        error = error + 1
                        print('Error : two incompatible definitions of A429 %s :\n%s\n%s' % (
                        cur_data.localName, cur_data, other))
                else:
                    curTypeNode[cur_data.localName] = cur_data

        return error, warning

class A429SamplingLabel(object):
    def __init__(self, number, sdi, localName, validityDurationUs, periodUs=None):
        self.number = number
        self.sdi = sdi
        self.localName = localName
        self.validityDurationUs = validityDurationUs
        self.periodUs = periodUs

    def convertToXml(self):
        el = ET.Element('SamplingLabel',
                        Number=self.number,
                        Sdi=self.sdi,
                        LocalName=self.localName,
                        ValidityDurationUs=self.validityDurationUs)
        if self.periodUs is not None:
            el.set('PeriodUs', self.periodUs)
        return el

    @staticmethod
    def parse(element):
        period = None
        if 'PeriodUs' in element.attrib:
            period = element.attrib['PeriodUs']

        return A429SamplingLabel(element.attrib['Number'],
                                 element.attrib['Sdi'],
                                 element.attrib['LocalName'],
                                 element.attrib['ValidityDurationUs'],
                                 period)

    def isCompatible(self, other):
        return self.number==other.number and self.sdi==other.sdi and self.localName==other.localName and self.validityDurationUs==other.validityDurationUs and self.periodUs==other.periodUs

    def __str__(self):
        return str(self.__dict__)

class A429QueuingLabel(object):
    def __init__(self, number, sdi, localName, queueDepth):
        self.number = number
        self.sdi = sdi
        self.localName = localName
        self.queueDepth = queueDepth

    def convertToXml(self):
        return ET.Element('QueuingLabel',Number=self.number,Sdi=self.sdi,LocalName=self.localName,QueueDepth=self.queueDepth)


    @staticmethod
    def parse(element):
        return A429QueuingLabel(element.attrib['Number'],
                                element.attrib['Sdi'],
                                element.attrib['LocalName'],
                                element.attrib['QueueDepth'])

    def isCompatible(self, other):
        return self.number == other.number and self.sdi == other.sdi and self.localName == other.localName and self.queueDepth == other.queueDepth

    def __str__(self):
        return str(self.__dict__)



##############################################
#                    CAN                     #
##############################################

class Can(object):
    def __init__(self):
        self.producedData = {}
        self.consumedData = {}

    def isEmpty(self):
        return not self.producedData and not self.consumedData

    def convertToXml(self):
        can_element = ET.Element('CAN')

        if self.producedData:
            can_prod_element = ET.SubElement(can_element, 'ProducedData')
            for bus in self.producedData.values():
                can_prod_element.append(bus.convertToXml())

        if self.consumedData:
            can_cons_element = ET.SubElement(can_element, 'ConsumedData')
            for bus in self.consumedData.values():
                can_cons_element.append(bus.convertToXml())

        return can_element

    @staticmethod
    def parse(element):
        can = Can()

        if element is None:
            return None

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = element.find(node_name)
            if data_direction_el is None:
                continue

            for bus_el in data_direction_el.findall('Bus'):
                if node_name == 'ConsumedData':
                    can.consumedData[bus_el.attrib['Name']] = CanBus.parse(bus_el)
                    curBusNode = can.consumedData[bus_el.attrib['Name']]
                else:
                    can.producedData[bus_el.attrib['Name']] = CanBus.parse(bus_el)
                    curBusNode = can.producedData[bus_el.attrib['Name']]

                for sampling_el in bus_el.findall('SamplingMessage'):
                    curBusNode.samplings[sampling_el.attrib['Id']] = CanSamplingMsg.parse(sampling_el)

                for queuing_el in bus_el.findall('QueuingMessage'):
                    curBusNode.queuings[queuing_el.attrib['Id']] = CanQueuingMsg.parse(queuing_el)

        return can

    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        if elementToMerge is None:
            return 0,0

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = elementToMerge.find(node_name)
            if data_direction_el is None:
                continue

            for bus_el in data_direction_el.findall('Bus'):
                if node_name == 'ConsumedData':
                    curBus = CanBus.parse(bus_el)
                    curDirNode = destCompoment.consumedData
                else:
                    curBus = CanBus.parse(bus_el)
                    curDirNode = destCompoment.producedData

                if curBus.name in curDirNode:
                    other = curDirNode[curBus.name]
                    err,warn = CanBus.merge(other, bus_el)
                    error+=err
                    warning+=warn
                else:
                    curDirNode[curBus.name] = curBus

        return error,warning

class CanBus(object):
    def __init__(self, name):
        self.name = name
        self.samplings = {}
        self.queuings = {}

    def convertToXml(self):
        bus_element = ET.Element('Bus', Name=self.name)

        for can in self.samplings.values():
            bus_element.append(can.convertToXml())
        for can in self.queuings.values():
            bus_element.append(can.convertToXml())

        return bus_element

    @staticmethod
    def parse(element):
        return CanBus(element.attrib['Name'])

    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        for node_type_name in ['SamplingMessage', 'QueuingMessage']:
            for element in elementToMerge.findall(node_type_name):

                if node_type_name == 'SamplingMessage':
                    curTypeNode = destCompoment.samplings
                    cur_data = CanSamplingMsg.parse(element)
                else:
                    warning += 1
                    print('Warning : Queuing message not supported for CAN')
                    continue

                if cur_data.id in curTypeNode:
                    other = curTypeNode[cur_data.id]
                    if not cur_data.isCompatible(other):
                        error = error + 1
                        print('Error : two incompatible definitions of CAN %s :\n%s\n%s' % (
                            cur_data.id, cur_data, other))
                else:
                    curTypeNode[cur_data.id] = cur_data

        return error, warning

class CanSamplingMsg(object):
    def __init__(self, id, localName, messageSizeBytes, validityDurationUs, periodUs=None):
        self.id = id
        self.localName = localName
        self.messageSizeBytes = messageSizeBytes
        self.validityDurationUs = validityDurationUs
        self.periodUs = periodUs

    def convertToXml(self):
        el = ET.Element('SamplingMessage',
                        Id=self.id,
                        LocalName=self.localName,
                        MessageSizeBytes=self.messageSizeBytes,
                        ValidityDurationUs=self.validityDurationUs)
        if self.periodUs is not None:
            el.set('PeriodUs', self.periodUs)
        return el

    @staticmethod
    def parse(element):
        period = None
        if 'PeriodUs' in element.attrib:
            period = element.attrib['PeriodUs']

        return CanSamplingMsg(element.attrib['Id'],
                                 element.attrib['LocalName'],
                                 element.attrib['MessageSizeBytes'],
                                 element.attrib['ValidityDurationUs'],
                                 period)

    def isCompatible(self, other):
        return self.id==other.id and self.localName==other.localName and self.messageSizeBytes==other.messageSizeBytes and self.validityDurationUs==other.validityDurationUs and self.periodUs==other.periodUs

    def __str__(self):
        return str(self.__dict__)

class CanQueuingMsg(object):
    def __init__(self):
        pass

    def convertToXml(self):
        pass

    @staticmethod
    def parse(element):
        pass

    def isCompatible(self, other):
        pass

    def __str__(self):
        return str(self.__dict__)



##############################################
#                 DISCRETE                   #
##############################################

class Discrete(object):
    def __init__(self):
        self.producedData = {}
        self.consumedData = {}

    def isEmpty(self):
        return not self.producedData and not self.consumedData

    def convertToXml(self):
        dis_element = ET.Element('DISCRETE')

        if self.producedData:
            dis_prod_element = ET.SubElement(dis_element, 'ProducedData')
            for signal in self.producedData.values():
                dis_prod_element.append(signal.convertToXml())

        if self.consumedData:
            dis_cons_element = ET.SubElement(dis_element, 'ConsumedData')
            for signal in self.consumedData.values():
                dis_cons_element.append(signal.convertToXml())

        return dis_element

    @staticmethod
    def parse(element):
        dis = Discrete()

        if element is None:
            return None

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = element.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curTypeNode = dis.consumedData
            else:
                curTypeNode = dis.producedData

            for signal_el in data_direction_el.findall('Signal'):
                curTypeNode[signal_el.attrib['Name']] = DiscreteSignal.parse(signal_el)

        return dis


    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        if elementToMerge is None:
            return 0,0

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = elementToMerge.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curDirNode = destCompoment.consumedData
            else:
                curDirNode = destCompoment.producedData

            for element in data_direction_el.findall('Signal'):
                cur_signal = DiscreteSignal.parse(element)

                if cur_signal.name in curDirNode:
                    other = curDirNode[cur_signal.name]
                    if not cur_signal.isCompatible(other):
                        error = error + 1
                        print('Error : two incompatible definitions of DISCRETE signals %s :\n%s\n%s' % (cur_signal.name, cur_signal, other))
                else:
                    curDirNode[cur_signal.name] = cur_signal

        return error,warning


class DiscreteSignal(object):
    def __init__(self, name, localName, trueState=None, falseState=None, HWPP=None, periodUs=None, validityDurationUs=None):
        self.name = name
        self.localName = localName
        self.trueState = trueState
        self.falseState = falseState
        self.HWPP = HWPP
        self.periodUs = periodUs
        self.validityDurationUs = validityDurationUs

    def convertToXml(self):
        el = ET.Element('Signal',Name=self.name,LocalName=self.localName)

        if self.trueState is not None:
            el.set('TrueState', self.trueState)

        if self.falseState is not None:
            el.set('FalseState', self.falseState)

        if self.HWPP is not None:
            el.set('HWPP', self.HWPP)

        if self.periodUs is not None:
            el.set('PeriodUs', self.periodUs)

        if self.validityDurationUs is not None:
            el.set('ValidityDurationUs', self.validityDurationUs)

        return el

    @staticmethod
    def parse(element):
        attributes = {'TrueState':None, 'FalseState':None, 'HWPP':None, 'PeriodUs':None, 'ValidityDurationUs':None}

        for key in attributes:
            if key in element.attrib:
                attributes[key] = element.attrib[key]

        return DiscreteSignal(element.attrib['Name'],
                     element.attrib['LocalName'],
                     attributes['TrueState'],
                     attributes['FalseState'],
                     attributes['HWPP'],
                     attributes['PeriodUs'],
                     attributes['ValidityDurationUs'])

    def isCompatible(self, other):
        return self.name==other.name and \
               self.localName==other.localName and \
               self.trueState==other.trueState and \
               self.falseState==other.falseState and \
               self.HWPP==other.HWPP and \
               self.periodUs==other.periodUs and \
               self.validityDurationUs==other.validityDurationUs

    def __str__(self):
        return str(self.__dict__)


##############################################
#                 ANALOGUE                   #
##############################################

class Analogue(object):
    def __init__(self):
        self.producedData = {}
        self.consumedData = {}

    def isEmpty(self):
        return not self.producedData and not self.consumedData

    def convertToXml(self):
        ana_element = ET.Element('ANALOGUE')

        if self.producedData:
            ana_prod_element = ET.SubElement(ana_element, 'ProducedData')
            for signal in self.producedData.values():
                ana_prod_element.append(signal.convertToXml())

        if self.consumedData:
            ana_cons_element = ET.SubElement(ana_element, 'ConsumedData')
            for signal in self.consumedData.values():
                ana_cons_element.append(signal.convertToXml())

        return ana_element

    @staticmethod
    def parse(element):
        ana = Analogue()

        if element is None:
            return None

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = element.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curTypeNode = ana.consumedData
            else:
                curTypeNode = ana.producedData

            for signal_el in data_direction_el.findall('Signal'):
                curTypeNode[signal_el.attrib['Name']] = AnalogueSignal.parse(signal_el)

        return ana


    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        if elementToMerge is None:
            return 0,0

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = elementToMerge.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curDirNode = destCompoment.consumedData
            else:
                curDirNode = destCompoment.producedData

            for element in data_direction_el.findall('Signal'):
                cur_signal = AnalogueSignal.parse(element)

                if cur_signal.name in curDirNode:
                    other = curDirNode[cur_signal.name]
                    if not cur_signal.isCompatible(other):
                        error = error + 1
                        print('Error : two incompatible definitions of ANALOGUE signals %s :\n%s\n%s' % (cur_signal.name, cur_signal, other))
                else:
                    curDirNode[cur_signal.name] = cur_signal

        return error,warning


class AnalogueSignal(object):
    def __init__(self, name, localName, scaleFactor, offset, periodUs=None, validityDurationUs=None):
        self.name = name
        self.localName = localName
        self.scaleFactor = scaleFactor
        self.offset = offset
        self.periodUs = periodUs
        self.validityDurationUs = validityDurationUs

    def convertToXml(self):
        el = ET.Element('Signal',Name=self.name,LocalName=self.localName,ScaleFactor=self.scaleFactor,Offset=self.offset)

        if self.periodUs is not None:
            el.set('PeriodUs', self.periodUs)

        if self.validityDurationUs is not None:
            el.set('ValidityDurationUs', self.validityDurationUs)

        return el

    @staticmethod
    def parse(element):
        attributes = {'PeriodUs':None, 'ValidityDurationUs':None}

        for key in attributes:
            if key in element.attrib:
                attributes[key] = element.attrib[key]

        return AnalogueSignal(element.attrib['Name'],
                     element.attrib['LocalName'],
                     element.attrib['ScaleFactor'],
                     element.attrib['Offset'],
                     attributes['PeriodUs'],
                     attributes['ValidityDurationUs'])

    def isCompatible(self, other):
        return self.name==other.name and \
               self.localName==other.localName and \
               self.scaleFactor==other.scaleFactor and \
               self.offset==other.offset and \
               self.periodUs==other.periodUs and \
               self.validityDurationUs==other.validityDurationUs

    def __str__(self):
        return str(self.__dict__)



##############################################
#                    NAD                     #
##############################################

class NAD(object):
    def __init__(self):
        self.producedData = {}
        self.consumedData = {}

    def isEmpty(self):
        return not self.producedData and not self.consumedData

    def convertToXml(self):
        nad_element = ET.Element('NAD')

        if self.producedData:
            nad_prod_element = ET.SubElement(nad_element, 'ProducedData')
            for msg in self.producedData.values():
                nad_prod_element.append(msg.convertToXml())

        if self.consumedData:
            nad_cons_element = ET.SubElement(nad_element, 'ConsumedData')
            for msg in self.consumedData.values():
                nad_cons_element.append(msg.convertToXml())

        return nad_element

    @staticmethod
    def parse(element):
        nad = NAD()

        if element is None:
            return None

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = element.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curTypeNode = nad.consumedData
            else:
                curTypeNode = nad.producedData

            for signal_el in data_direction_el.findall('Message'):
                curTypeNode[signal_el.attrib['Name']] = NADMessage.parse(signal_el)

        return nad


    @staticmethod
    def merge(destCompoment, elementToMerge):
        error = 0
        warning = 0

        if elementToMerge is None:
            return 0,0

        for node_name in ['ConsumedData', 'ProducedData']:
            data_direction_el = elementToMerge.find(node_name)
            if data_direction_el is None:
                continue

            if node_name == 'ConsumedData':
                curDirNode = destCompoment.consumedData
            else:
                curDirNode = destCompoment.producedData

            for element in data_direction_el.findall('Message'):
                cur_msg = NADMessage.parse(element)

                if cur_msg.name in curDirNode:
                    other = curDirNode[cur_msg.name]
                    if not cur_msg.isCompatible(other):
                        error = error + 1
                        print('Error : two incompatible definitions of NAD message %s :\n%s\n%s' % (cur_msg.name, cur_msg, other))
                else:
                    curDirNode[cur_msg.name] = cur_msg

        return error,warning


class NADMessage(object):
    def __init__(self, name, localName, messageSizeBytes, periodUs=None, messageType=None, messageDimension1=None, messageDimension2=None):
        self.name = name
        self.localName = localName
        self.messageSizeBytes = messageSizeBytes
        self.periodUs = periodUs
        self.messageType = messageType
        self.messageDimension1 = messageDimension1
        self.messageDimension2 = messageDimension2

    def convertToXml(self):
        el = ET.Element('Message',Name=self.name,LocalName=self.localName,MessageSizeBytes=self.messageSizeBytes)

        if self.periodUs is not None:
            el.set('PeriodUs', self.periodUs)

        if self.messageType is not None:
            el.set('MessageType', self.messageType)

        if self.messageDimension1 is not None:
            el.set('MessageDimension1', self.messageDimension1)

        if self.messageDimension2 is not None:
            el.set('MessageDimension2', self.messageDimension2)

        return el

    @staticmethod
    def parse(element):
        attributes = {'PeriodUs':None, 'MessageType':None, 'MessageDimension1':None,'MessageDimension2':None}

        for key in attributes:
            if key in element.attrib:
                attributes[key] = element.attrib[key]

        return NADMessage(element.attrib['Name'],
                     element.attrib['LocalName'],
                     element.attrib['MessageSizeBytes'],
                     attributes['PeriodUs'],
                     attributes['MessageType'],
                     attributes['MessageDimension1'],
                     attributes['MessageDimension2'])

    def isCompatible(self, other):
        return self.name==other.name and \
               self.localName==other.localName and \
               self.messageSizeBytes==other.messageSizeBytes and \
               self.periodUs==other.periodUs and \
               self.messageDimension1==other.messageDimension1 and \
               self.messageDimension2==other.messageDimension2

    def __str__(self):
        return str(self.__dict__)