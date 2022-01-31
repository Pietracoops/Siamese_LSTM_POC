from siamese_lstm.dataset.quora_dataset import QuoraDataset, collate
from siamese_lstm.dataset.utils import convert_data_to_tuples, text_to_wordlist
from siamese_lstm.dataset.language import Language
from siamese_lstm.model.network import EmbeddingLSTMNet, SiameseNetwork
from siamese_lstm.model.model_trainer import ModelTrainer
from siamese_lstm.model.utils import create_pretrained_weights, plotConfusionMatrix, save_model
import os
import pandas as pd
import numpy as np
import wandb

import torch
import torch.nn as nn

from torch.utils.data import DataLoader, SubsetRandomSampler


# Root and directory paths
ROOT_PATH = os.getcwd()
DATA_FOLDER_PATH = ROOT_PATH + '/data'
GOOGLE_EMBEDDING_FOLDER_PATH = DATA_FOLDER_PATH + '/google embedding'
GOOGLE_EMBEDDING = GOOGLE_EMBEDDING_FOLDER_PATH + '/GoogleNews-vectors-negative300.bin'
MODEL_FOLDER_PATH = ROOT_PATH
# important paths
DATASET_FILE_PATH = DATA_FOLDER_PATH + '/Database.csv'
EMBEDDING_PATH = GOOGLE_EMBEDDING_FOLDER_PATH + GOOGLE_EMBEDDING

# general variables
EMBEDDING_REQUIRES_GRAD = False
NUM_LAYERS = 1


# hyperperameters
# adjust them to optimize the model performance

hparams = {
    'threshold': torch.Tensor([0.5]),  # threshold for determining similiarity
    'learning_rate': 1e-03,  # learning rate
    'epoch': 1,  # number of epochs
    'batch_size': 32,  # batch_size
    'hidden_dim': 100,  # number of hidden dimension
    'embedding_dim': 30,  # number of embedding dimension
    'dropout': 0.0,  # dropout
    'remove_stopwords': False,  # removes stopwords
    'stem_words': False,  # remove stem words
    'simple': True,  # chooses simple or more complex model
    'log_to_wandb': False,  # chooses to log to wandb or not
}


# Load the Data
df = pd.read_csv(DATASET_FILE_PATH)


# Select the questions pairs and labels and prepare the data
q_pair, labels = convert_data_to_tuples(df)


# Create a language
language = Language()
for data in [q_pair]:
    for question_pair in data:
        q1 = question_pair[0]
        q2 = question_pair[1]
        language.addSentence(q1)
        language.addSentence(q2)


# Create the Pytorch Dataset
quora_dataset = QuoraDataset(q_pair, language.word2index, labels)


# example output

for sample in quora_dataset:
    print('question 1:', sample['q1'])
    print('question 2:', sample['q2'])
    print('tokens  q1:', sample['q1_token'])
    print('tokens  q2:', sample['q2_token'])
    print('labels    :', sample['labels'])
    break


# Data loader

train_split = 0.8
val_split = 0.2

dataset_size = len(quora_dataset)
indices = list(range(dataset_size))

split_train = int(train_split*dataset_size)

shuffle_dataset = True
random_seed = 46

if shuffle_dataset :
    np.random.seed(random_seed)
    np.random.shuffle(indices)
train_indices, val_indices = indices[:split_train], indices[split_train:]

assert len(train_indices) + len(val_indices) == dataset_size

train_sampler = SubsetRandomSampler(train_indices)
val_sampler = SubsetRandomSampler(val_indices)

train_dataloader = torch.utils.data.DataLoader(quora_dataset, batch_size=hparams['batch_size'], sampler=train_sampler, collate_fn=collate)
val_dataloader = torch.utils.data.DataLoader(quora_dataset, batch_size=hparams['batch_size'], sampler=val_sampler, collate_fn=collate)

print('Training Set Size {}, Validation Set Size {},'.format(len(train_indices), len(val_indices)))


# MODEL

# create pretrained weights
# skip this step because time consuming and might make cpu brake
# instead import the pretrained_weights.pt

pretrained_weights = create_pretrained_weights(hparams['embedding_dim'], language)
pretrained_weights


# pretrained_weights = torch.load('pretrained_weights.pt')
# pretrained_weights

# embedding net
embedding_net = EmbeddingLSTMNet(
    hparams['embedding_dim'],
    hparams['hidden_dim'],
    NUM_LAYERS,
    EMBEDDING_REQUIRES_GRAD,
    pretrained_weights,
    hparams['dropout'],
    hparams['simple'], # if simple=True --> simple model, if simple=False --> more complex model (2 linear layers plus relu)
)

# siamese model
model = SiameseNetwork(embedding_net)

# test model class with one batch from the dataloader
for i, batch in enumerate(train_dataloader):
    q1, q2 = batch['q1_token'], batch['q2_token']
    q1_len, q2_len = batch['q1_lengths'], batch['q2_lengths']
    y = torch.FloatTensor(batch['labels'])

model(q1, q2, q1_len, q2_len)

# TRAINING

# Initialize the model trainer
trainer = ModelTrainer(
    model,
    hparams,
    train_dataloader,
    val_dataloader,
    train_indices,
    val_indices,
    log_to_wandb=hparams['log_to_wandb'],
)


# Fit the model
trainer.fit()

# Evaluation
trainer.test()


# If you like to store the model, uncomment the following lines and enter filename and path

#filename = "model.pt"
#path = ROOT_PATH
#save_model(model, path + "/" + filename)


# MAKE PREDICTIONS

# Checkout random sample from training data
ind = np.random.choice(len(train_dataloader))
test_sample_train = dict()
for idx, batch in enumerate(train_dataloader):
    if idx == ind:
        test_sample_train['q1_text'] = [batch['q1_text'][0]]
        test_sample_train['q2_text'] = [batch['q2_text'][0]]
        test_sample_train['q1_token'] = [batch['q1_token'][0]]
        test_sample_train['q2_token'] = [batch['q2_token'][0]]
        test_sample_train['q1_lengths'] = [batch['q1_lengths'][0]]
        test_sample_train['q2_lengths'] = [batch['q2_lengths'][0]]
        test_sample_train['labels'] = [batch['labels'][0]]

trainer.predict(test_sample_train)

# Checkout random sample from validation data
ind = np.random.choice(len(val_dataloader))

test_sample_val = dict()
for idx, batch in enumerate(val_dataloader):
    if idx == ind:
        test_sample_val['q1_text'] = [batch['q1_text'][0]]
        test_sample_val['q2_text'] = [batch['q2_text'][0]]
        test_sample_val['q1_token'] = [batch['q1_token'][0]]
        test_sample_val['q2_token'] = [batch['q2_token'][0]]
        test_sample_val['q1_lengths'] = [batch['q1_lengths'][0]]
        test_sample_val['q2_lengths'] = [batch['q2_lengths'][0]]
        test_sample_val['labels'] = [batch['labels'][0]]

trainer.predict(test_sample_val)



# Prediction from custom input questions

default = True  # set to False to create your own inputs
similar = False  # select False to dispaly dissimlar example

if default:
    if similar:
        q1 = ['Is it cold today?']
        q2 = ['Will it be cold today?']
        label = [1.0]
    else:
        q1 = ['Will I pass the final?']
        q2 = ['What will I have for dinner tonight?']
        label = [0.0]
else:
    q1 = input(r'Enter your 1^st question: ')
    q2 = input(r'Enter your 2^nd question: ')
    label = input('label: ') # 0 for dissimilar, 1 for similar
    q1 = [q1]
    q2 = [q2]
    label = [label]


# prepair custom input
# create df
df_own = pd.DataFrame(list(zip(q1, q2, label)), columns=['question1', 'question2', 'is_duplicate'])
# prepare data
q_pair_own, label = convert_data_to_tuples(df_own, hparams['remove_stopwords'], hparams['stem_words'])
print(q_pair_own)

# create dataset
own_dataset = QuoraDataset(q_pair_own, language.word2index, label)

# create dataloader
predict_dataloader = torch.utils.data.DataLoader(own_dataset, batch_size=1, collate_fn=collate)

for sample in predict_dataloader:
    test_sample = sample

# predict
trainer.predict(test_sample)
