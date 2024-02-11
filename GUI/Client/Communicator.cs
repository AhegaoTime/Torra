using System;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace Client
{

    // You create one of the classes from the serialization file, and then send it with the SendRequest() function.
    // And then you receive the response to one of the classes in the serialization file.
    public class Communicator
    {
        private Socket? _socket;
        public Communicator()
        {
            try
            {
                // Establish the remote endpoint
                // for the socket. This example
                // uses port 8826 on the local
                // computer.
                IPAddress ipAddr = Dns.GetHostAddresses("localhost").Where(x => x.AddressFamily == AddressFamily.InterNetwork).First();
                IPEndPoint localEndPoint = new IPEndPoint(ipAddr, 8826);

                // Creation TCP/IP Socket using
                // Socket Class Constructor
                this._socket = new Socket(ipAddr.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

                try
                {
                    this._socket.Connect(localEndPoint);



                }

                // Manage of Socket's Exceptions
                catch (ArgumentNullException ane)
                {

                    Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
                }

                catch (SocketException se)
                {

                    Console.WriteLine("SocketException : {0}", se.ToString());
                }

                catch (Exception e)
                {
                    Console.WriteLine("Unexpected exception : {0}", e.ToString());
                }
            }
            catch (Exception e)
            {

                Console.WriteLine(e.ToString());
            }
        }

        public void CloseSocket()
        {
            if (this._socket != null)
            {
                this._socket.Shutdown(SocketShutdown.Both);
                this._socket.Close();
            }
        }
        public void SendRequest<T>(T? Request)
        {
            if (this._socket == null)
            {
                throw new Exception("Socket is null.");
            }

            byte[] messageSent = Serialization.SerializeRequest<T>(Request);
            int byteSent = this._socket.Send(messageSent);
            if (byteSent != messageSent.Length)
            {
                //oops.
                throw new Exception("Not all bytes sent.");
            }
        }

        public T? ReceiveResponse<T>()
        {
            if (this._socket == null)
            {
                throw new Exception("Socket is null.");
            }

            byte[] responseHeaders = new byte[5];
            this._socket.Receive(responseHeaders);

            if (responseHeaders[0] != 200)
            {
                throw new Exception("Error Response Received.");
            }

            byte[] responseSize = responseHeaders.Skip(1).ToArray();
            Array.Reverse(responseSize);
            uint size = BitConverter.ToUInt32(responseSize);
            System.Diagnostics.Debug.WriteLine("Size: ", size.ToString());
            byte[] responseData = new byte[size];
            this._socket.Receive(responseData);

            string jsonString = System.Text.Encoding.UTF8.GetString(responseData);

            System.Diagnostics.Debug.WriteLine("Response: ", jsonString);

            return Deserialization.DesirializeResponse<T>(jsonString);
        }
    }
}
