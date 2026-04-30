
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(CONNECTION_LISTENER)
#define CONNECTION_LISTENER

class ConnectionListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual void connectionClosed(bool abort=false) = 0;
};

#endif