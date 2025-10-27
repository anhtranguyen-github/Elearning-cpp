# Protocol

# Standard Principles for Designing Application Protocols over Sockets

### **1. Consistency and Mutual Agreement**

- Both client and server must **understand and follow the same communication rules**.
- The protocol must be **unambiguous** (no confusion in interpretation) and **complete** (covering all communication cases).

---

### **2. Clear Message Structure**

Each message should consist of two parts:

- **Header** – identifies the message type and contains control or meta information (fixed length recommended).
- **Payload** – carries the actual data content.

**Rules:**

- Every message type must have a **well-defined format, length, and meaning**.
- Choose between **byte-based** or **text-based** message formats depending on efficiency and debugging needs.
- Define a **clear end-of-message indicator**, such as `\n`, `\r\n`, or length-based framing.

---

### **3. Message Encoding**

- Use a consistent encoding scheme: **binary** or **textual**.
- For text-based protocols, follow a line-based format:
    
    ```
    <MESSAGE_TYPE> <field1> <field2> ...
    
    ```
    
- For binary protocols, define **byte order (endianness)** and **exact byte length** for each field.

---

### **4. Message Type Identification**

- Each message must have a **unique identifier** (e.g., numeric code or keyword).
- Identifiers should be **short, fixed-length, and descriptive**.
- Maintain a mapping table such as:
    
    ```
    0x01 → LOGIN
    0x02 → MOVE
    0x03 → STATE_UPDATE
    0x04 → RESULT
    
    ```
    
- This ensures consistent message parsing and interpretation.

---

### **5. Communication Sequence Compliance**

- Communication must follow the **predefined message sequence or state machine**.
- Each step should specify:
    - Which side initiates (client or server).
    - What message type is sent/received.
    - What action or state transition occurs next.
- Use a **state table** to describe transitions between communication states.

---

### **6. Message Processing Rules**

- When receiving a message:
    - Validate type, format, and length.
    - Process valid messages and respond accordingly.
    - Send an error code if invalid.
- When sending a message:
    - Format it correctly.
    - Send it in the proper order as defined in the sequence.

---

### **7. Reply and Error Handling**

- Each request should have a corresponding reply.
- Use **consistent response codes** for success and error cases.
    
    Example:
    
    ```
    11 → LOGIN_SUCCESS
    12 → LOGIN_FAILED
    21 → TEXT_DELIVERED
    22 → SENDER_NOT_LOGGED_IN
    
    ```
    
- Both client and server must handle error codes appropriately.

---

### **8. State Management**

- Clearly define whether the protocol is **stateful** or **stateless**.
- Stateful protocols maintain user/session state (e.g., login status, game state).
- Stateless protocols must include all required data in each message so that processing does not depend on previous exchanges.

---

### **9. Connection and Transport Control**

- Specify the transport layer:
    - **Reliable (TCP)** – ensures ordering and data integrity.
    - **Unreliable (UDP)** – may need manual acknowledgment.
- Implement proper **connection setup, maintenance, and termination** messages (e.g., `QUIT`, `DISCONNECT`).

---

### **10. Extensibility and Compatibility**

- The protocol should be **extensible**, allowing new message types without breaking existing functionality.
- Maintain **version information** to ensure backward compatibility.
- Update documentation alongside implementation changes.

---

### **11. Testing and Logging**

- Log every message sent and received for debugging and validation.
- Verify:
    - Correct message format.
    - Correct message order.
    - Proper responses and error handling.
- Test the system on **two separate physical machines** to confirm network stability.

---

### **12. Basic Security**

- Avoid sending sensitive data (e.g., passwords) in plaintext.
- Apply encryption or encoding (e.g., Base64 or TLS if applicable).
- Validate all incoming data to prevent injection or buffer overflow attacks.