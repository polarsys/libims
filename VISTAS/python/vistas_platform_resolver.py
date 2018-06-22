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

import sys
import os
import xml.etree.ElementTree as ET


class VistasChannel(object):
    def __init__(self, channel_type, name, direction, fifosize, messagemaxsize, src_id, sn, qos_timestamp,
                 data_timestamp, signals):
        self.Type = channel_type
        self.Name = name
        self.Direction = direction
        self.FifoSize = fifosize
        self.MessageMaxSize = messagemaxsize
        self.Src_Id = src_id
        self.SN = sn
        self.QoS_Timestamp = qos_timestamp
        self.Data_Timestamp = data_timestamp
        self.Signals = signals

        self.IP = None
        self.Port = None

    def is_compatible(self, other):
        return self.Type == other.Type and self.FifoSize == other.FifoSize and self.MessageMaxSize == other.MessageMaxSize and self.Signals == other.Signals

    def has_same_header(self, other):
        return self.Src_Id == other.Src_Id and self.SN == other.SN and self.QoS_Timestamp == other.QoS_Timestamp and self.Data_Timestamp == other.Data_Timestamp


    def __str__(self):
        return str(self.__dict__)


class VirtualComponent(object):
    def __init__(self, name):
        self.name = name
        self.input_channels = {}
        self.output_channels = {}


def parse_channel_element(element):
    name = element.attrib['Name']
    channel_type = element.tag
    direction = element.attrib['Direction']
    fifosize = element.attrib['FifoSize']
    messagemaxsize = element.attrib['MessageMaxSize']
    src_id = 'No'
    sn = 'No'
    qos_timestamp = 'No'
    data_timestamp = 'No'
    attributes = ['Type','Size','Unit']
    for header in element.findall('Header'):
        src_id = header.attrib['Src_Id']
        sn = header.attrib['SN']
        qos_timestamp = header.attrib['QoS_Timestamp']
        data_timestamp = header.attrib['Data_Timestamp']
    signals = {}
    for sig_element in element.findall('Signals/Signal'):
        signals[sig_element.attrib['Name']] = (sig_element.attrib['ByteOffset'],)

        for key in attributes:
            if key in sig_element.attrib:
                signals[sig_element.attrib['Name']] += (sig_element.attrib[key],)
    return VistasChannel(channel_type, name, direction, fifosize, messagemaxsize, src_id, sn, qos_timestamp,
                         data_timestamp, signals)

def convert_channel_to_element(vc, direction, channel):
    element = ET.SubElement(vc, channel.Type, Name=channel.Name, Direction=direction,
                            MessageMaxSize=channel.MessageMaxSize, FifoSize=channel.FifoSize)
    ET.SubElement(element, 'Socket', DstIP=channel.IP, DstPort=channel.Port)
    ET.SubElement(element, 'Header', Src_Id=channel.Src_Id, SN=channel.SN, QoS_Timestamp=channel.QoS_Timestamp, Data_Timestamp=channel.Data_Timestamp)
    if len(channel.Signals) > 0:
        signals = ET.SubElement(element, 'Signals')
        for (key, value) in channel.Signals.items():
            el = ET.SubElement(signals, 'Signal', Name=key, ByteOffset=value[0])

            if len(value) >= 2:
                el.set('Type',value[1])
            if len(value) >= 3:
                el.set('Size',value[2])
            if len(value) >= 4:
                el.set('Unit',value[3])
    return element


# because ElementTree does not know how to indent...
def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level + 1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(
            'Usage: vistas_platform_resolver.py merged_vistas_output.xml vistas_input1.xml [vistas_input2.xml vistas_input3.xml ...]')
        sys.exit(1)

    vistas_output_path = sys.argv[1]

    vistas_input_path_lst = []
    for input_path in sys.argv[2:]:
        if os.path.isfile(input_path):
            vistas_input_path_lst.append(input_path)
        else:
            print('Input file not found : ' + input_path)
            sys.exit(1)

    # key : str with channel name, value : VistasChannel
    all_channels = {}
    # key : str with vc name, value : VirtualComponent
    all_vc = {}
    # key : str with channel name, value : str with vc name of the producer
    all_produced_channels = {}

    num_warnings = 0
    num_errors = 0

    type_map = {'A429_Channel': ("224.225.100.0", 6000), 'A664_Channel': ("224.226.100.0", 6000),
                'A825_Channel': ("224.227.100.0", 6000), 'MIL_BUS_Channel': ("224.228.100.0", 6000),
                'Discrete_Channel': ("224.229.100.0", 6000), 'Analog_Channel': ("224.230.100.0", 6000),
                'NAD_Channel': ("224.231.100.0", 6000)}

    for input_path in vistas_input_path_lst:
        print('Reading file : ' + input_path)
        input_tree = ET.parse(input_path)
        input_root = input_tree.getroot()

        for vc_element in input_root.findall('VirtualComponent'):
            vc_name = vc_element.attrib['Name']
            print('VirtualComponent : ' + vc_name)
            if vc_name in all_vc:
                print('Error : several VirtualComponents have the name ' + vc_name)
                num_errors = num_errors + 1
                vc = all_vc[vc_name]
            else:
                vc = VirtualComponent(vc_name)
                all_vc[vc_name] = vc

            for channel_element in vc_element:
                channel = parse_channel_element(channel_element)
                direction = channel.Direction

                if channel.Type in type_map:
                    (ip, port) = type_map[channel.Type]

                    for socket in channel_element.findall('Socket'):
                        socket.set('DstIP', ip)
                        socket.set('DstPort', str(port))

                    channel.IP = ip
                    channel.Port = str(port)
                    type_map[channel.Type] = (ip, port + 1)

                # is the channel compatible with an already defined one ?
                if channel.Name in all_channels:
                    other_channel = all_channels[channel.Name]
                    if not channel.is_compatible(other_channel):
                        print('Error : two incompatible definitions of channel %s :\n%s\n%s' % (
                            channel.Name, channel, other_channel))
                        num_errors = num_errors + 1
                    elif not channel.has_same_header(other_channel):
                        print('Warning : two incompatible definitions of header for channel %s :\n%s\n%s' % (
                            channel.Name, channel, other_channel))
                        num_warnings = num_warnings + 1


                    # keep new definition if new one is "Out" and old one is "In"
                    if other_channel.Direction == 'In' and channel.Direction == 'Out':
                        other_channel.Src_Id = channel.Src_Id
                        other_channel.SN = channel.SN
                        other_channel.QoS_Timestamp = channel.QoS_Timestamp
                        other_channel.Data_Timestamp = channel.Data_Timestamp

                    channel = other_channel
                else:
                    all_channels[channel.Name] = channel

                # is the channel already produced ?
                if direction == 'Out':
                    if channel.Name in vc.output_channels:
                        print('Warning : the channel %s is already produced by the same VirtualComponent' % (
                            channel.Name))
                        num_warnings = num_warnings + 1
                    elif channel.Name in all_produced_channels:
                        print(
                            'Warning : the channel %s is already produced by VirtualComponent %s' % (
                                channel.Name, all_produced_channels[channel.Name]))
                        num_warnings = num_warnings + 1

                    all_produced_channels[channel.Name] = vc.name
                    vc.output_channels[channel.Name] = channel

                # is the channel already consumed ?
                if direction == 'In':
                    if channel.Name in vc.input_channels:
                        print('Warning : the channel %s is already consumed by the same VirtualComponent' % (
                            channel.Name))
                        num_warnings = num_warnings + 1

                    vc.input_channels[channel.Name] = channel

        indent(input_root)
        tree = ET.ElementTree(input_root)

        filename, file_extension = os.path.splitext(input_path)
        tree.write(filename + '_resolved' + file_extension, encoding='utf-8', xml_declaration=True)

    # for channel in all_channels.values():
    #     if channel.Type in type_map:
    #         (ip, port) = type_map[channel.Type]
    #         channel.IP = ip
    #         channel.Port = str(port)
    #         type_map[channel.Type] = (ip, port + 1)
    #     else:
    #         print('Error : unknown type for channel ' + channel.Name)
    #         num_errors = num_errors + 1

    print('Errors : ' + str(num_errors))
    print('Warnings : ' + str(num_warnings))

    if num_errors > 0:
        print('There were errors, aborting')
        sys.exit(1)

    print('Writing file : ' + vistas_output_path)

    network_element = ET.Element('Network')
    for vc in all_vc.values():
        vc_element = ET.SubElement(network_element, 'VirtualComponent', Name=vc.name)
        for channel in vc.input_channels.values():
            convert_channel_to_element(vc_element, "In", channel)
        for channel in vc.output_channels.values():
            convert_channel_to_element(vc_element, "Out", channel)

    indent(network_element)

    tree = ET.ElementTree(network_element)
    tree.write(vistas_output_path, encoding='utf-8', xml_declaration=True)

    print('Resolve done')
    sys.exit(0)
