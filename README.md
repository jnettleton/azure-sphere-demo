# azure-sphere-demo

## Sources

The ```AzureIoT``` folder was copied from https://github.com/Azure/azure-sphere-samples/tree/main/Samples/AzureIoT .
- The ```AzureIoT/common/main.c``` file was removed and select parts were merged with ```main.c``` .
- The ```AzureIoT/common/exitcodes.h``` file was removed to prevent duplicates.
- In ```AzureIoT/common``` , both ```cloud.h``` and ```cloud.c``` were modified to account for telemetry requirements.
- Copied May 5 2022 @ commit [2a040e9](https://github.com/Azure/azure-sphere-samples/tree/2a040e92f8873924da81e24ce144bed2e063d85c/Samples/AzureIoT)
