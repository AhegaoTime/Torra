using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Client
{
    public class LoginRequest //done
    {
        public string? Username { get; set; }
        public string? Password { get; set; }
    }
    
    public class SignupRequest //done
    {
        public string? Username { get; set; }
        public string? Password { get; set; }
    }

    public class SignoutRequest //done
    {
        public string? Username { get; set; }
        public string? Password { get; set; }
    }

    public class CreateChatRequest //done
    {
        public string? Username { get; set; }

        public string? Password { get; set; }
    }

    public class SendMsgRequest
    {
        public string? Username { get; set; } // other user

        public string? Password { get; set; }// main user
        public string? Msg { get; set; }// msg
    }

    public class CloseChatRequest
    {
        public string? Username { get; set; }

        public string? Password { get; set; }
    }
    public class GuiRequest
    {
    }
    public class Serialization
    {
        public static Dictionary<string, byte> Codes = new Dictionary<string, byte>()
        {
            {"SignupRequest", 11},
            {"LoginRequest", 12},
            {"CreateChatRequest", 31},
            {"SignoutRequest", 32},
            {"SendMsgRequest", 41},
            {"CloseChatRequest", 42},
            {"ChatMsgUpdateRequest", 43},
            {"GuiRequest", 91 }
            
        };
        public static byte[] SerializeRequest<T>(T request)
        {
            string jsonString = JsonConvert.SerializeObject(request);
            System.Diagnostics.Debug.WriteLine("jsonString: ", jsonString);
            byte[] jsonArray = Encoding.UTF8.GetBytes(jsonString);
            UInt32 length = (uint)jsonString.Length;
            byte[] lengthBytes = BitConverter.GetBytes(length);
            byte[] message = new byte[5 + length];
            System.Diagnostics.Debug.WriteLine(typeof(T).Name);
            message[0] = Codes[typeof(T).Name];
            Array.Reverse(lengthBytes);
            message[1] = lengthBytes[0];
            message[2] = lengthBytes[1];
            message[3] = lengthBytes[2];
            message[4] = lengthBytes[3];

            for (uint counter = 0; counter < length; counter++)
            {
                message[counter + 5] = jsonArray[counter];
            }

            return message;
        }
    }
}
