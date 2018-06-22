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
from ims_type import AFDX, A429, Can, Discrete, Analogue, NAD

class DataExchange(object):
    def __init__(self):
        self.afdx = AFDX()
        self.a429 = A429()
        self.can = Can()
        self.discrete = Discrete()
        self.analogue = Analogue()
        self.nad = NAD()

    def convertToXml(self):
        data_ex_element = ET.Element('DataExchange')

        # write AFDX node
        if self.afdx != None and not self.afdx.isEmpty():
            data_ex_element.append(self.afdx.convertToXml())

        # write A429 node
        if self.a429 != None and not self.a429.isEmpty():
            data_ex_element.append(self.a429.convertToXml())

        # write CAN node
        if self.can != None and not self.can.isEmpty():
            data_ex_element.append(self.can.convertToXml())

        # write DISCRETE node
        if self.discrete != None and not self.discrete.isEmpty():
            data_ex_element.append(self.discrete.convertToXml())

        # write ANALOGUE node
        if self.analogue != None and not self.analogue.isEmpty():
            data_ex_element.append(self.analogue.convertToXml())

        # write NAD node
        if self.nad != None and not self.nad.isEmpty():
            data_ex_element.append(self.nad.convertToXml())

        return data_ex_element


    @staticmethod
    def parse(element):
        if element is None:
            return None

        dataExchange = DataExchange()
        dataExchange.afdx = AFDX.parse(element.find("AFDX"))
        dataExchange.a429 = A429.parse(element.find("A429"))
        dataExchange.can = Can.parse(element.find("CAN"))
        dataExchange.discrete = Discrete.parse(element.find("DISCRETE"))
        dataExchange.analogue = Analogue.parse(element.find("ANALOGUE"))
        dataExchange.nad = NAD.parse(element.find("NAD"))

        return dataExchange

    @staticmethod
    def merge(destComponent, elementToMerge):
        errorAfdx, warningAfdx = AFDX.merge(destComponent.afdx, elementToMerge.find("AFDX"))
        errorA429, warningA429 = A429.merge(destComponent.a429, elementToMerge.find("A429"))
        errorCan, warningCan = Can.merge(destComponent.can, elementToMerge.find("CAN"))
        errorDis, warningDis = Discrete.merge(destComponent.discrete, elementToMerge.find("DISCRETE"))
        errorAna, warningAna = Analogue.merge(destComponent.analogue, elementToMerge.find("ANALOGUE"))
        errorNad, warningNad = NAD.merge(destComponent.nad, elementToMerge.find("NAD"))

        error = errorAfdx + errorA429 + errorCan + errorDis + errorAna + errorNad
        warning = warningAfdx + warningA429 + warningCan + warningDis + warningAna + warningAna

        return error,warning


class Equipment(object):
    def __init__(self, name):
        self.name = name
        self.dataExchange = DataExchange()
        self.applications = {}

    @staticmethod
    def parse(element):
        if element is None:
            return None

        equipment = Equipment(element.attrib['Name'])
        applications = {}

        dataExchange = DataExchange.parse(element.find('DataExchange'))

        for app_element in element.findall('Application'):
            app_name = app_element.attrib['Name']
            applications[app_name] = Application.parse(app_element)

        equipment.dataExchange = dataExchange
        equipment.applications = applications

        return equipment


    def convertToXml(self):
        equipment_el = ET.Element('Equipment', Name=self.name)

        if self.dataExchange is not None:
            equipment_el.append(self.dataExchange.convertToXml())

        for app in self.applications.values():
            equipment_el.append(app.convertToXml())

        return equipment_el


    @staticmethod
    def merge(destComponent, elementToMerge):
        print("Merge of equipment \'" + destComponent.name + "\'")
        errorApp = 0
        warningApp = 0

        if elementToMerge is None:
            return 0, 0

        dataExchange_element = elementToMerge.find('DataExchange')
        error_da,warning_da = DataExchange.merge(destComponent.dataExchange, dataExchange_element)

        for app_element in elementToMerge.findall('Application'):
            app_name = app_element.attrib['Name']
            err,warn=Application.merge(destComponent.applications[app_name], app_element)
            errorApp+=err
            warningApp+=warn

        error = error_da + errorApp
        warning = warning_da + warningApp

        return error, warning


class Application(object):
    def __init__(self, name):
        self.name = name
        self.dataExchange = DataExchange()

    @staticmethod
    def parse(element):
        if element is None:
            return None

        application = Application(element.attrib['Name'])
        dataExchange = None

        dataExchange_el = element.find('DataExchange')
        if dataExchange_el is not None:
            dataExchange = DataExchange.parse(dataExchange_el)

        application.dataExchange = dataExchange

        return application


    @staticmethod
    def merge(destComponent, elementToMerge):
        print("Merge of Application \'" + destComponent.name + "\'")
        if elementToMerge is None:
            return 0, 0

        dataExchange_el = elementToMerge.find('DataExchange')

        return DataExchange.merge(destComponent.dataExchange,dataExchange_el)


    def convertToXml(self):
        application_el = ET.Element('Application', Name=self.name)

        if self.dataExchange is not None:
            application_el.append(self.dataExchange.convertToXml())

        return application_el


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
            'Usage: ims_platform_resolver.py merged_ims_output.xml ims_input1.xml [ims_input2.xml ims_input3.xml ...]')
        sys.exit(1)

    rootVcName = ''

    ims_output_path = sys.argv[1]

    ims_input_path_lst = []
    for input_path in sys.argv[2:]:
        if os.path.isfile(input_path):
            ims_input_path_lst.append(input_path)
        else:
            print('Input file not found : ' + input_path)
            sys.exit(1)

    # the unique root data exchange
    global_data_exchange = None

    # key : str with eq name, value : Equipment
    all_eq = {}

    num_warnings = 0
    num_errors = 0

    for input_path in ims_input_path_lst:
        print('Reading file : ' + input_path)
        input_tree = ET.parse(input_path)
        input_root = input_tree.getroot()

        rootVcName += input_root.attrib['Name']
        if input_path != ims_input_path_lst[len(ims_input_path_lst)-1]:
            rootVcName += '_'

        # DATAEXCHANGE NODE PARSE
        da_element = input_root.find('DataExchange')
        if da_element is not None:
            if global_data_exchange is not None:
                errors,warnings = DataExchange.merge(global_data_exchange, da_element)
                num_errors += errors
                num_warnings += warnings
            else:
                global_data_exchange = DataExchange.parse(da_element)

        # EQUIPMENT PARSE
        for eq_element in input_root.findall('Equipment'):
            eq_name = eq_element.attrib['Name']
            if eq_name in all_eq:
                eq = all_eq[eq_name]
                errors, warnings = Equipment.merge(eq, eq_element)
                num_errors += errors
                num_warnings += warnings
            else:
                eq = Equipment(eq_name)
                all_eq[eq_name] = Equipment.parse(eq_element)

    print('Errors : ' + str(num_errors))
    print('Warnings : ' + str(num_warnings))

    if num_errors > 0:
        print('There were errors, aborting')
        sys.exit(1)

    print('Writing file : ' + ims_output_path)

    vc_element = ET.Element('VirtualComponent', Name=rootVcName)

    if global_data_exchange is not None:
        vc_element.append(global_data_exchange.convertToXml())

    for eq in all_eq.values():
        vc_element.append(eq.convertToXml())

    indent(vc_element)

    tree = ET.ElementTree(vc_element)
    tree.write(ims_output_path, encoding='utf-8', xml_declaration=True)

    print('Resolve done')
    sys.exit(0)
