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
import xml.etree.ElementTree as ET


def invert_vistas(root):
    for el in root.iter():
        if el.tag == 'VirtualComponent':
            el.attrib['Name'] = 'Inverted' + el.attrib['Name']
        elif 'Direction' in el.attrib:
            direction = el.attrib['Direction']
            if direction == 'In':
                el.attrib['Direction'] = 'Out'
            elif direction == 'Out':
                el.attrib['Direction'] = 'In'


def invert_cfg(root):
    for el in root.iter():
        if el.tag == 'ConsumedData':
            el.tag = 'ProducedData'
        elif el.tag == 'ProducedData':
            el.tag = 'ConsumedData'


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print('Usage: vistas_inverter.py vistas.xml cfg.xml inverted_vistas.xml inverted_cfg.xml')
        sys.exit(1)

    orig_vistas = sys.argv[1]
    orig_cfg = sys.argv[2]
    inv_vistas = sys.argv[3]
    inv_cfg = sys.argv[4]

    # read the files

    print('Reading ' + orig_vistas)
    vistas_tree = ET.parse(orig_vistas)

    print('Reading ' + orig_cfg)
    cfg_tree = ET.parse(orig_cfg)

    # modify the documents
    invert_vistas(vistas_tree.getroot())
    invert_cfg(cfg_tree.getroot())

    # write the files
    print('Writing ' + inv_vistas)
    vistas_tree.write(inv_vistas, encoding='utf-8', xml_declaration=True)

    print('Writing ' + inv_cfg)
    cfg_tree.write(inv_cfg, encoding='utf-8', xml_declaration=True)

    print('Inversion finished')
    sys.exit(0)
