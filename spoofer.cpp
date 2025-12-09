#include "HwidSpoofer.h"
#include "SystemRoutines.h"

#include <ntddstor.h>
#include <Ntdddisk.h>


// ============================================
//      HWID SPOOFER - CONSOLE INTERFACE
// ============================================
//  Основной файл консольного приложения
//  Содержит пользовательский интерфейс и управление
//  Разработано: Yasimmy Dev
// ============================================


PDRIVER_DISPATCH OldIrpMj;
char NumTable[] = "123456789";
char SpoofedHWID[] = "          XXXYYYYYYX\0";
bool HWIDGenerated = false;
ULONG SpoofCount = 0;
ULONG SmartBlockCount = 0;

VOID ResetHWIDGeneration()
{
	HWIDGenerated = false;
	memcpy(SpoofedHWID, "          XXXYYYYYYX\0", 21);
	DbgPrint("[HWID SPOOFER] Флаг генерации сброшен - следующий HWID будет новым\n");
}

VOID SpoofSerialNumber(char* serialNumber)
{
	if (!HWIDGenerated)
	{
		HWIDGenerated = true;

		LARGE_INTEGER Seed;
		KeQuerySystemTimePrecise(&Seed);

		char templateHWID[] = "          XXXYYYYYYX\0";
		memcpy(SpoofedHWID, templateHWID, 21);

		for (int i = 0; i < strlen(SpoofedHWID); ++i)
		{
			if (SpoofedHWID[i] == 'Y')
			{
				SpoofedHWID[i] = RtlRandomEx(&Seed.LowPart) % 26 + 65;
			}

			if (SpoofedHWID[i] == 'X')
			{
				SpoofedHWID[i] = NumTable[RtlRandomEx(&Seed.LowPart) % (strlen(NumTable) - 1)];
			}
		}

		DbgPrint("\n");
		DbgPrint("╔══════════════════════════════════════════════════════════╗\n");
		DbgPrint("║     [HWID SPOOFER] Генерация нового Hardware ID         ║\n");
		DbgPrint("╠══════════════════════════════════════════════════════════╣\n");
		DbgPrint("║  Статус: [OK] Генерация завершена успешно               ║\n");
		DbgPrint("║  Новый HWID: %-40s ║\n", SpoofedHWID);
		DbgPrint("╚══════════════════════════════════════════════════════════╝\n");
	}

	SpoofCount++;
	memcpy((void*)serialNumber,
		(void*)SpoofedHWID,
		21);

	if (SpoofCount % 10 == 0)
	{
		DbgPrint("[SPOOF] Серийный номер подменен [Всего: %lu]\n", SpoofCount);
	}
}

struct REQUEST_STRUCT
{
	PIO_COMPLETION_ROUTINE OldRoutine;
	PVOID OldContext;
	ULONG OutputBufferLength;
	PSTORAGE_DEVICE_DESCRIPTOR StorageDescriptor;
};

NTSTATUS StorageQueryCompletionRoutine(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	PVOID Context
)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);

	PIO_COMPLETION_ROUTINE OldCompletionRoutine = NULL;
	PVOID OldContext = NULL;
	ULONG OutputBufferLength = 0;
	PSTORAGE_DEVICE_DESCRIPTOR descriptor = NULL;

	if (Context != NULL)
	{
		REQUEST_STRUCT* pRequest = (REQUEST_STRUCT*)Context;
		OldCompletionRoutine = pRequest->OldRoutine;
		OldContext = pRequest->OldContext;
		OutputBufferLength = pRequest->OutputBufferLength;
		descriptor = pRequest->StorageDescriptor;

		ExFreePool(Context);
	}

	if (FIELD_OFFSET(STORAGE_DEVICE_DESCRIPTOR, SerialNumberOffset) < OutputBufferLength &&
		descriptor->SerialNumberOffset > 0 &&
		descriptor->SerialNumberOffset < OutputBufferLength)
	{
		char* SerialNumber = ((char*)descriptor) + descriptor->SerialNumberOffset;
		
		DbgPrint("[INTERCEPT] Запрос серийного номера обнаружен -> Подмена...\n");

		SpoofSerialNumber(SerialNumber);

		DbgPrint("[SUCCESS] Серийный номер успешно подменен: %s\n", SerialNumber);
	}
	else
	{
		DbgPrint("[WARNING] Некорректный дескриптор устройства (Offset: %lu, Length: %lu)\n", 
			descriptor ? descriptor->SerialNumberOffset : 0, OutputBufferLength);
	}

	if ((Irp->StackCount > (ULONG)1) && (OldCompletionRoutine != NULL))
		return OldCompletionRoutine(DeviceObject, Irp, OldContext);
	
	return STATUS_SUCCESS;
}

NTSTATUS SmartCompletionRoutine(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	PVOID Context
)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_COMPLETION_ROUTINE OldCompletionRoutine = NULL;
	PVOID OldContext = NULL;

	if (Context != NULL)
	{
		REQUEST_STRUCT* pRequest = (REQUEST_STRUCT*)Context;
		OldCompletionRoutine = pRequest->OldRoutine;
		OldContext = pRequest->OldContext;
		ExFreePool(Context);
	}

	SmartBlockCount++;
	
	if (SmartBlockCount % 5 == 0)
	{
		DbgPrint("[BLOCK] SMART запрос заблокирован [Всего заблокировано: %lu]\n", SmartBlockCount);
	}

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	
	return Irp->IoStatus.Status;
}

NTSTATUS HookedMjDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	PIO_STACK_LOCATION Ioc = IoGetCurrentIrpStackLocation(Irp);

	switch (Ioc->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_STORAGE_QUERY_PROPERTY:
	{
		PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)Irp->AssociatedIrp.SystemBuffer;

		if (query->PropertyId == StorageDeviceProperty)
		{
			DbgPrint("[HOOK] IOCTL_STORAGE_QUERY_PROPERTY перехвачен -> Установка completion routine\n");

			Ioc->Control = 0;
			Ioc->Control |= SL_INVOKE_ON_SUCCESS;

			PVOID OldContext = Ioc->Context;
			Ioc->Context = (PVOID)ExAllocatePool(NonPagedPool, sizeof(REQUEST_STRUCT));
			REQUEST_STRUCT *pRequest = (REQUEST_STRUCT*)Ioc->Context;
			pRequest->OldRoutine = Ioc->CompletionRoutine;
			pRequest->OldContext = OldContext;
			pRequest->OutputBufferLength = Ioc->Parameters.DeviceIoControl.OutputBufferLength;
			pRequest->StorageDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)Irp->AssociatedIrp.SystemBuffer;

			Ioc->CompletionRoutine = (PIO_COMPLETION_ROUTINE)StorageQueryCompletionRoutine;
		}

		break;

	}
	case SMART_RCV_DRIVE_DATA:
	{
		DbgPrint("[HOOK] SMART_RCV_DRIVE_DATA перехвачен -> Блокировка запроса\n");

		Ioc->Control = 0;
		Ioc->Control |= SL_INVOKE_ON_SUCCESS;

		PVOID OldContext = Ioc->Context;
		Ioc->Context = (PVOID)ExAllocatePool(NonPagedPool, sizeof(REQUEST_STRUCT));
		REQUEST_STRUCT *pRequest = (REQUEST_STRUCT*)Ioc->Context;
		pRequest->OldRoutine = Ioc->CompletionRoutine;
		pRequest->OldContext = OldContext;

		Ioc->CompletionRoutine = (PIO_COMPLETION_ROUTINE)SmartCompletionRoutine;

		break;
	}
	}

	return OldIrpMj(DeviceObject, Irp);

}

NTSTATUS IrpHookDisk()
{
	PDRIVER_OBJECT hookDriver = NULL;

	UNICODE_STRING unDriverName;
	RtlInitUnicodeString(&unDriverName, L"\\Driver\\Disk");

	auto Status = ObReferenceObjectByName(&unDriverName,
		OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL,
		(PVOID*)&hookDriver);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("\n");
		DbgPrint("╔══════════════════════════════════════════════════════════╗\n");
		DbgPrint("║              [HWID SPOOFER] ОШИБКА ИНИЦИАЛИЗАЦИИ        ║\n");
		DbgPrint("╠══════════════════════════════════════════════════════════╣\n");
		DbgPrint("║  Статус: [FAILED] Не удалось получить объект драйвера  ║\n");
		DbgPrint("║  Код ошибки: 0x%08X                                    ║\n", Status);
		DbgPrint("╚══════════════════════════════════════════════════════════╝\n");
		return Status;
	}

	OldIrpMj = hookDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL];
	hookDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)HookedMjDeviceControl;

	DbgPrint("\n");
	DbgPrint("╔══════════════════════════════════════════════════════════╗\n");
	DbgPrint("║          [HWID SPOOFER] УСПЕШНАЯ ИНИЦИАЛИЗАЦИЯ           ║\n");
	DbgPrint("╠══════════════════════════════════════════════════════════╣\n");
	DbgPrint("║  Статус: [OK] Драйвер Disk успешно перехвачен            ║\n");
	DbgPrint("║  Хук установлен: IRP_MJ_DEVICE_CONTROL                  ║\n");
	DbgPrint("║  Оригинальный обработчик сохранен                         ║\n");
	DbgPrint("║  Система готова к работе                                 ║\n");
	DbgPrint("╚══════════════════════════════════════════════════════════╝\n");

	return Status;

}
