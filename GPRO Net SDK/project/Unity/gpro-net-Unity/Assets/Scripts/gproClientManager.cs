using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class gproClientManager : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
		int test = gproClientPlugin.foo(9000);
		Debug.Log(test);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
