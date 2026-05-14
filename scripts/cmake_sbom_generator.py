#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 SBOM Generator
# SPDX-License-Identifier: Apache-2.0
"""
CMake SBOM Generator
Generates SBOM from CMake build information

inspired by
https://sbomgenerator.com/guides/cpp#cmake-integration-for-sbom-tracking
"""
import uuid
import json
import re
import argparse
import subprocess
from pathlib import Path
import datetime
import logging

# cyclonedx license and references information
components = [
    {
      "group": "Tencent", "name": "rapidjson", "version": "1.1.0",
      "type": "library",
      "licenses": [{ "license": { "id": "MIT" } }],
      "externalReferences": [
        { "type": "website", "url": "https://github.com/Tencent/rapidjson" }
      ],
      "properties": [
        { "name": "cdx:integration:type", "value": "header-only" }
      ]
    },
    {
      "group": "Thalhammer", "name": "jwt-cpp", "version": "0.7.2",
      "type": "library", "bom-ref": "jwt-cpp",
      "licenses": [{"license": {"id": "MIT"}}],
      "externalReferences": [
        {"type": "website", "url": "https://github.com/Thalhammer/jwt-cpp"}
      ],
      "properties": [
        {"name": "cdx:integration:type", "value": "header-only" }
      ]
    },
    {
      "group": "Contributors to log4cplus Project", "name": "log4cplus", "version": "2.1.2",
      "type": "library", "bom-ref": "log4cplus",
      "licenses": [{"license": {"id": "Apache-2.0"}}],
      "externalReferences": [
          {"type": "website", "url": "https://github.com/log4cplus/log4cplus"}]
    },
    {
      "group": "The Qt Company", "name": "Qt6Core", "version": "6.10.3",
      "type": "library","bom-ref": "Qt6Core",
      "licenses": [{"license": {"id": "LGPL-3.0-only"}}]
    },
    {
        "group": "The ZeroMQ authors", "name": "zmq", "version": "4.3.5",
        "type": "library",
        "licenses": [{"license": {"id": "MPL-2.0"}}],
        "externalReferences": [
            {"type": "website", "url": "https://github.com/zeromq/libzmq"}]
    },
    {
        "group": "Free Software Foundation, Inc", "name": "gettext", "version": "0.25",
        "type": "library",
        "licenses": [{"license": {"id": "LGPL-3.0-or-later"}}],
        "externalReferences": [
          {"type": "website", "url": "https://www.gnu.org/software/gettext"}]
    },
    {
        "group": "Daniel Stenberg, <daniel@haxx.se>", "name": "curl", "version": "8.19.0",
        "type": "library",
        "licenses": [{"license": {"id": "curl"}}],
        "externalReferences": [
          {"type": "website", "url": "https://curl.se"}]
    },
    {
        "group": "The Libxml2 Contributors", "name": "libxml2", "version": "2.14.5",
        "type": "library",
        "licenses": [{"license": {"id": "MIT"}}],
        "externalReferences": [
          {"type": "website", "url": "https://gitlab.gnome.org/GNOME/libxml2/-/wikis/home"}]
    },
    {
        "group": "Daniel Veillard", "name": "libxslt", "version": "1.1.45",
        "type": "library",
        "licenses": [{"license": {"id": "MIT"}}],
        "externalReferences": [
          {"type": "website", "url": "https://gitlab.gnome.org/GNOME/libxslt"}]
    },
    {
        "group": "Baptiste Lepilleur and The JsonCpp", "name": "jsoncpp", "version": "1.9.6",
        "type": "library",
        "licenses": [{"license": {"id": "MIT"}}],
        "externalReferences": [
          {"type": "website", "url": "https://github.com/open-source-parsers/jsoncpp/"}]
    },
    {
        "group": "Josef Wilgen, Uwe Rathmann", "name": "gwt", "version": "6.3.0",
        "type": "library",
        "licenses": [{"license": {"id": "Qwt exception 1.0"}}],
        "externalReferences": [
          {"type": "website", "url": "https://qwt.sourceforge.io"}]
    },
    {
        "group": "Jean-loup Gailly and Mark Adler", "name": "zlib", "version": "1.3.2",
        "type": "library",
        "licenses": [{"license": {"id": "Zlib"}}],
        "externalReferences": [
          {"type": "website", "url": "https://zlib.net"}]
    }
]

class CMakeSBOMGenerator:
    def __init__(self, build_dir, target_name):
        self.build_dir = Path(build_dir)
        self.target_name = target_name

    def generate_sbom(self, output_file):
        """Generate SBOM from CMake build information"""

        # Load base metadata
        metadata_file = self.build_dir / "sbom_metadata.json"
        with open(metadata_file) as f:
            metadata = json.load(f)

        # Load dependencies
        dependencies = {k: v for k,v in [e.split(';')
                                         for e in Path(self.build_dir / "dependencies.txt").read_text().split()]}
        logging.info("dependencies %s", dependencies)
        # append header
        # Create CycloneDX document
        sbom = {
            "bomFormat": "CycloneDX",
            "specVersion": "1.7",
            "serialNumber": f"urn:uuid:{str(uuid.uuid4())}",
            "version": 1,
            "metadata": {
                "timestamp": str(datetime.datetime.now(datetime.UTC).replace(microsecond=0).isoformat()),
                "tools": {
                    "components": [
                        {
                            "group": "ch.semafor",
                            "name": "cmake-sbom-generator",
                            "version": metadata['project']['version'],
                            "type": "application",
                            "bom-ref": "pkg:generic/cmake-sbom-generator",
                            "publisher": "SEMAFOR Informatik & Energie AG",
                            "authors": [
                                {
                                    "name": "SEMAFOR Informatik & Energie AG"
                                }
                            ]
                        }
                    ]
                },
                "authors": [
                    {
                        "name": "SEMAFOR Informatik & Energie AG"
                    }
                ]
            },
            "lifecycles": [
                {
                    "phase": "build"
                }
            ],
            "component": {
                "group": "ch.semafor",
                "name": f"{self.target_name}",
                "version": metadata['project']['version'],
                "type": "application",
                "purl": "pkg:generic/intens",
                "bom-ref": "pkg:generic/intens"
            },
            "components": []
        }

        for comp in components:
            if 'properties' in comp:
                if next(filter(lambda p: p.get('value', '') == 'header-only',
                               comp['properties']), {}):
                    sbom['components'].append(comp)

        for k in dependencies:
            comp = next(filter(
                lambda c: c['name'] == k, components), {})
            if comp:
                version = dependencies[k]
                if version:
                    comp['version'] = version
                sbom["components"].append(comp)
                
        # Write SBOM
        try:
            with open(output_file, 'w') as f:
                json.dump(sbom, f, indent=2)
        except TypeError as e:
            logging.error("%s: %s", output_file, sbom)
            raise e
        print(f"Generated CMake SBOM: {output_file}")


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s')
    parser = argparse.ArgumentParser(description="Generate SBOM from CMake build")
    parser.add_argument("--build-dir", required=True, help="CMake build directory")
    parser.add_argument("--target", required=True, help="CMake target name")
    parser.add_argument("--output", required=True, help="Output SBOM file")

    args = parser.parse_args()

    generator = CMakeSBOMGenerator(args.build_dir, args.target)
    generator.generate_sbom(args.output)
