# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
#
# SPDX-License-Identifier: Apache-2.0

### make install ###
install(
  PROGRAMS intens-gen update-database.py create-changelogs.py
  DESTINATION bin
  )
