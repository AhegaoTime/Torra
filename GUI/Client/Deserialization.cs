using System.Collections.Generic;
using Newtonsoft.Json;

namespace Client
{
    //Status is 0 - Success, 1 - Failure.
    public class ErrorResponse
    {
        public string? Message { get; set; }
    }

    public class LoginResponse
    {
        public bool Status { get; set; }
    }

    public class SignupResponse // done
    {
        public bool Status { get; set; }
    }

    public class SignoutResponse
    {
        public bool Status { get; set; }
    }
    public class CreateChatResponse
    {
        public bool Status { get; set; }
    }
    public class SendMsgResponse
    {
        public bool Status { get; set; }
    }
    public class ChatResponseResponse
    { 
        public bool Status { get; set; } 
        public string Message { get; set; }
    }
    public class CloseChatResponse
    {
        public bool Status { get; set; }
    }
    public class Deserialization
    {
        //If this works than I love C#
        //Like, that's insane
        public static T? DesirializeResponse<T>(string jsonString)
        {
            return JsonConvert.DeserializeObject<T>(jsonString);
        }
    }
}
